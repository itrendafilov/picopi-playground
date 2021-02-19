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

static void moveLineTextLeft(int oldY, int newY, int val) {
    Graphics_Rectangle blackZone = {
           .xMin = 22,
           .xMax = 50,
           .yMin = oldY - 6,
           .yMax = oldY + 5
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
    Graphics_drawLineH(&g_sContext, 10, 19, oldY);
    Graphics_fillRectangle(&g_sContext, &blackZone);

    // Color is cached for LineHCustom
    Graphics_drawLineH(&g_sContext, 10, 19, newY);
    Graphics_setForegroundColor(&g_sContext, ClrWhite);

    int c = 3, x = 52;
    char *ptr = &text[2];
    while(c-- > 0) {
        x -= ((char)*ptr) == '1' ? 9 : 10;
//        Graphics_setForegroundColor(&g_sContext, ClrBlack);
//        Graphics_fillRectangle(&g_sContext, &blackZone);


        if (*ptr != ' ') {
            Graphics_copyDisplayCustom(x, newY - 5, (int8_t)(*ptr) - '0', &g_sContext, &g_vdContext);
        }
        ptr--;
//
//        blackZone.xMax = blackZone.xMin - 1;
//        blackZone.xMin -= 10;
    }
}

void APP_altispeed_zone5_redraw(int16_t airspeed) {
    const Graphics_Rectangle clippingZone1 = {
           .xMin = 10,
           .xMax = 59,
           .yMin = 0,
           .yMax = 99
    };

    const Graphics_Rectangle clippingZone2 = {
           .xMin = 10,
           .xMax = 59,
           .yMin = 141,
           .yMax = 239
    };

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);

    int centerSpeed = divBy10(airspeed);
    int fraction = airspeed - centerSpeed * 10;
    int prevY = divBy10(prevAirspeed);
    int prevFraction = prevAirspeed - prevY * 10;
    prevAirspeed = airspeed;

    prevFraction >>= 1;
    prevY = modBy10(prevY);
    prevY *= INSTRUMENT_ALITSPEED_PPK;
    prevY += 120 + prevFraction; // this will be the previosDelta

    fraction >>= 1;
    int16_t deltaSpeed = modBy10(centerSpeed);
    int16_t currY = 120 + fraction + deltaSpeed * INSTRUMENT_ALITSPEED_PPK;
    centerSpeed -= deltaSpeed;

    // Move prevY before currY as we don't want to delete what we just printed :D
    if (prevY < 120) {
        Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone1);
    } else {
        Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone2);
    }
    if (prevY >= currY + 25) {
        moveLineTextLeft(prevY, prevY, -1);
        prevY -= 50;
    } else if (prevY <= currY - 25) {
        moveLineTextLeft(prevY, prevY, -1);
        prevY += 50;
    }

    // Now start drawing
    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone2);
    moveLineTextLeft(prevY, currY, centerSpeed); // ~0
    prevY += 50; currY += 50; centerSpeed -= 10;
    moveLineTextLeft(prevY, currY, centerSpeed); // -10
    prevY += 50; currY += 50; centerSpeed -= 10;
    moveLineTextLeft(prevY, currY, centerSpeed); // -20

    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone1);
    prevY -=150; currY -=150; centerSpeed += 30;
    moveLineTextLeft(prevY, currY, centerSpeed); // +10
    prevY -= 50; currY -= 50; centerSpeed += 10;
    moveLineTextLeft(prevY, currY, centerSpeed); // +20
    prevY -= 50; currY -= 50; centerSpeed += 10;
    moveLineTextLeft(prevY, currY, centerSpeed); // +30

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
