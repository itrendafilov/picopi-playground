/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include "displays/displays.h"
#include "displays/drivers/vd_driver.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevAirspeed;

void APP_altispeed_zone5_reset() {
    prevAirspeed = -1;
}

static void moveLineTextLeft(int oldX, int newX, int val) {
    Graphics_Rectangle blackZone = {
           .yMin = 22,
           .yMax = 50,
           .xMin = oldX - 6,
           .xMax = oldX + 5
    };
    char text[3] = { ' ', ' ', ' '};
    if (val >= 0) {
        text[2] = '0' + modBy10(val);
    }
    if (val > 9) {
        val = divBy10(val);
        text[1] = '0' + modBy10(val);
    }
    if (val > 9) {
        val = divBy10(val);
        text[0] = '0' + modBy10(val);
    }

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_drawLineV(&g_sContext, oldX, 10, 19);
    Graphics_fillRectangle(&g_sContext, &blackZone);

    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawLineV(&g_sContext, newX, 10, 19);

    int c = 3, y = 52;
    char *ptr = &text[2];
    while(c-- > 0) {
        y -= ((char)*ptr) == '1' ? 9 : 10;
//        Graphics_setForegroundColor(&g_sContext, ClrBlack);
//        Graphics_fillRectangle(&g_sContext, &blackZone);


        if (*ptr != ' ') {
            Graphics_copyDisplayCustom(newX - 5, y, (int8_t)(*ptr) - '0', &g_sContext, &g_vdContext);
        }
        ptr--;
//
//        blackZone.xMax = blackZone.xMin - 1;
//        blackZone.xMin -= 10;
    }
}

void APP_altispeed_zone5_redraw(int16_t airspeed) {
    const Graphics_Rectangle clippingZone1 = {
           .yMin = 10,
           .yMax = 59,
           .xMin = 0,
           .xMax = 99
    };

    const Graphics_Rectangle clippingZone2 = {
           .yMin = 10,
           .yMax = 59,
           .xMin = 141,
           .xMax = 239
    };

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);

    int centerSpeed = divBy10(airspeed);
    int fraction = airspeed - centerSpeed * 10;
    int prevX = divBy10(prevAirspeed);
    int prevFraction = prevAirspeed - prevX * 10;
    prevAirspeed = airspeed;

    prevFraction >>= 1;
    prevX = modBy10(prevX);
    prevX *= INSTRUMENT_ALITSPEED_PPK;
    prevX += 120 + prevFraction; // this will be the previosDelta

    fraction >>= 1;
    int16_t deltaSpeed = modBy10(centerSpeed);
    int16_t currX = 120 + fraction + deltaSpeed * INSTRUMENT_ALITSPEED_PPK;
    centerSpeed -= deltaSpeed;

    // Move prevX before currX as we don't want to delete what we just printed :D
    if (prevX < 120) {
        Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone1);
    } else {
        Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone2);
    }
    if (prevX >= currX + 25) {
        moveLineTextLeft(prevX, prevX, -1);
        prevX -= 50;
    } else if (prevX <= currX - 25) {
        moveLineTextLeft(prevX, prevX, -1);
        prevX += 50;
    }

    // Now start drawing
    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone2);
    moveLineTextLeft(prevX, currX, centerSpeed); // ~0
    prevX += 50; currX += 50; centerSpeed -= 10;
    moveLineTextLeft(prevX, currX, centerSpeed); // -10
    prevX += 50; currX += 50; centerSpeed -= 10;
    moveLineTextLeft(prevX, currX, centerSpeed); // -20

    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone1);
    prevX -=150; currX -=150; centerSpeed += 30;
    moveLineTextLeft(prevX, currX, centerSpeed); // +10
    prevX -= 50; currX -= 50; centerSpeed += 10;
    moveLineTextLeft(prevX, currX, centerSpeed); // +20
    prevX -= 50; currX -= 50; centerSpeed += 10;
    moveLineTextLeft(prevX, currX, centerSpeed); // +30

    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&noScreenClipping);
}

APP_DrawPriority APP_altispeed_zone5_check(int16_t airspeed) {
    int delta = abs(airspeed - prevAirspeed);

    if (delta < 2) {  // there is some movement, but it won't change the pixels on screen
        return DRAW_NOTHING;
    } else if (delta < 10) { // only the fraction changed
        return COULD_DRAW;
    } else {  // changes are big enough
        return MUST_DRAW;
    }
}
