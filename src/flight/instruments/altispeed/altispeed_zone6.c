/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include <speedmath.h>
#include "displays/displays.h"
#include "displays/drivers/vd_driver.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevAltitudeA;
static int16_t prevAltitudeB;

void APP_altispeed_zone6a_reset() {
    prevAltitudeA = -1;
}

void APP_altispeed_zone6b_reset() {
    prevAltitudeB = -1;
}

static void moveLineTextRight(int oldY, int newY, int val) {
    Graphics_Rectangle blackZone = {
           .xMin = 288,
           .xMax = 298,
           .yMin = oldY - 6,
           .yMax = oldY + 5
    };
    if (val < 0) val = 0;
    else val *= INSTRUMENT_ALITSPEED_PPFD;
    bcd32_t bcdVal = hex2bcd(val);
    char text[5] = { val < 100 ? ' ' : '0' + ((bcdVal & 0x0F00) >> 8),
                      val < 10 ? ' ' : '0' + ((bcdVal & 0x00F0) >> 4),
                         '0' + (bcdVal & 0x000F), '0', '0'};
//    char text[5] = {val < 10 ? ' ' : '0' + ((bcdVal & 0x00F0) >> 4),
//                         '0' + (bcdVal & 0x000F), '0', '0', '0' };


    int c = 5, x = 300;
    char *ptr = &text[4];
    while(c-- > 0) {
        x -= ((char)*ptr) == '1' ? 9 : 10;
        Graphics_setForegroundColor(&g_sContext, ClrBlack);
        Graphics_fillRectangle(&g_sContext, &blackZone);

        if (*ptr != ' ') {
            Graphics_copyDisplayCustom(x, newY - 5, (int8_t)(*ptr) - '0', &g_sContext, &g_vdContext);
        }
        ptr--;

        blackZone.xMax = blackZone.xMin - 1;
        blackZone.xMin -= 10;
    }
}


void APP_altispeed_zone6a_redraw(int16_t altitude) {
    const Graphics_Rectangle clippingZone1 = {
           .xMin = 250,
           .xMax = 319,
           .yMin = 0,
           .yMax = 89
    };

    int val = INSTRUMENT_ALTISPEED_ATOD(altitude);

    int currY = INSTRUMENT_ALTISPEED_START_YC(altitude);
    int prevY = INSTRUMENT_ALTISPEED_START_YC(prevAltitudeA);
    // Move prevY before currY as we don't want to delete what we just printed :D
    if (prevY >= currY + (INSTRUMENT_ALITSPEED_PPFX >> 1)) prevY -= INSTRUMENT_ALITSPEED_PPFX;

    prevAltitudeA = altitude;

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);

    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone1);

    for (; currY > -1 * INSTRUMENT_ALITSPEED_PPF || prevY > -1 * INSTRUMENT_ALITSPEED_PPF; // up to one minor division
        /* step */
            val++, currY -= INSTRUMENT_ALITSPEED_PPFX, prevY -= INSTRUMENT_ALITSPEED_PPFX
    ) {
        moveLineTextRight(prevY, currY, val);
    }
    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&noScreenClipping);
}


void APP_altispeed_zone6b_redraw(int16_t altitude) {
    const Graphics_Rectangle clippingZone2 = {
           .xMin = 250,
           .xMax = 319,
           .yMin = 151,
           .yMax = 239
    };

    int val = INSTRUMENT_ALTISPEED_ATOD(altitude) - 2;

    int currY = INSTRUMENT_ALTISPEED_START_Y(altitude);
    int prevY = INSTRUMENT_ALTISPEED_START_Y(prevAltitudeB);
    // Move prevY before currY as we don't want to delete what we just printed :D
    if (prevY >= currY + (INSTRUMENT_ALITSPEED_PPFX >> 1)) prevY -= INSTRUMENT_ALITSPEED_PPFX;


    prevAltitudeB = altitude;

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);
    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone2);
    for (; currY > 120 || prevY > 120;
        /* step */
            val++, currY -= INSTRUMENT_ALITSPEED_PPFX, prevY -= INSTRUMENT_ALITSPEED_PPFX
    ) {
        moveLineTextRight(prevY, currY, val);
    }

    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&noScreenClipping);
}

APP_DrawPriority APP_altispeed_zone6a_check(int16_t altitude) {
    int delta = abs(altitude - prevAltitudeA);

    // 25 = there is some movement, but it won't change the pixels on screen
    // +25 = we can skip one and still point at the proper text
    // +13 = midpoint between the two, so we draw even less frequently ;)
    if (delta < INSTRUMENT_ALTISPEED_APPD(1)) {
        return DRAW_NOTHING;
    } else if (delta < 5 * INSTRUMENT_ALTISPEED_APPD(1)) { // very small movement
        return COULD_DRAW;
    } else {
        return MUST_DRAW;
    }
}

APP_DrawPriority APP_altispeed_zone6b_check(int16_t altitude) {
    int delta = abs(altitude - prevAltitudeB);

    // 25 = there is some movement, but it won't change the pixels on screen
    // +25 = we can skip one and still point at the proper text
    // +13 = midpoint between the two, so we draw even less frequently ;)
    if (delta < INSTRUMENT_ALTISPEED_APPD(1)) {
        return DRAW_NOTHING;
    } else if (delta < 5 * INSTRUMENT_ALTISPEED_APPD(1)) { // very small movement
        return COULD_DRAW;
    } else {
        return MUST_DRAW;
    }
}
