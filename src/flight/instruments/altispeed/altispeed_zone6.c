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

static void moveLineTextRight(int oldX, int newX, int val) {
    Graphics_Rectangle blackZone = {
           .yMin = 288,
           .yMax = 298,
           .xMin = oldX - 6,
           .xMax = oldX + 5
    };
    if (val < 0) val = 0;
    else val *= INSTRUMENT_ALITSPEED_PPFD;
    bcd32_t bcdVal = hex2bcd(val);
    char text[5] = { val < 100 ? ' ' : '0' + ((bcdVal & 0x0F00) >> 8),
                      val < 10 ? ' ' : '0' + ((bcdVal & 0x00F0) >> 4),
                         '0' + (bcdVal & 0x000F), '0', '0'};
//    char text[5] = {val < 10 ? ' ' : '0' + ((bcdVal & 0x00F0) >> 4),
//                         '0' + (bcdVal & 0x000F), '0', '0', '0' };


    int c = 5, y = 300;
    char *ptr = &text[4];
    while(c-- > 0) {
        y -= ((char)*ptr) == '1' ? 9 : 10;
        Graphics_setForegroundColor(&g_sContext, ClrBlack);
        Graphics_fillRectangle(&g_sContext, &blackZone);

        if (*ptr != ' ') {
            Graphics_copyDisplayCustom(newX - 5, y, (int8_t)(*ptr) - '0', &g_sContext, &g_vdContext);
        }
        ptr--;

        blackZone.yMax = blackZone.yMin - 1;
        blackZone.yMin -= 10;
    }
}


void APP_altispeed_zone6a_redraw(int16_t altitude) {
    const Graphics_Rectangle clippingZone1 = {
           .yMin = 250,
           .yMax = 319,
           .xMin = 0,
           .xMax = 89
    };

    int val = INSTRUMENT_ALTISPEED_ATOD(altitude);

    int currX = INSTRUMENT_ALTISPEED_START_XC(altitude);
    int prevX = INSTRUMENT_ALTISPEED_START_XC(prevAltitudeA);
    // Move prevX before currX as we don't want to delete what we just printed :D
    if (prevX >= currX + (INSTRUMENT_ALITSPEED_PPFY >> 1)) prevX -= INSTRUMENT_ALITSPEED_PPFY;

    prevAltitudeA = altitude;

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);

    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone1);

    for (; currX > -1 * INSTRUMENT_ALITSPEED_PPF || prevX > -1 * INSTRUMENT_ALITSPEED_PPF; // up to one minor division
        /* step */
            val++, currX -= INSTRUMENT_ALITSPEED_PPFY, prevX -= INSTRUMENT_ALITSPEED_PPFY
    ) {
        moveLineTextRight(prevX, currX, val);
    }
    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&noScreenClipping);
}


void APP_altispeed_zone6b_redraw(int16_t altitude) {
    const Graphics_Rectangle clippingZone2 = {
           .yMin = 250,
           .yMax = 319,
           .xMin = 151,
           .xMax = 239
    };

    int val = INSTRUMENT_ALTISPEED_ATOD(altitude) - 2;

    int currX = INSTRUMENT_ALTISPEED_START_X(altitude);
    int prevX = INSTRUMENT_ALTISPEED_START_X(prevAltitudeB);
    // Move prevX before currX as we don't want to delete what we just printed :D
    if (prevX >= currX + (INSTRUMENT_ALITSPEED_PPFY >> 1)) prevX -= INSTRUMENT_ALITSPEED_PPFY;


    prevAltitudeB = altitude;

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);
    Graphics_setClipRegion(&g_sContext, (Graphics_Rectangle*)&clippingZone2);
    for (; currX > 120 || prevX > 120;
        /* step */
            val++, currX -= INSTRUMENT_ALITSPEED_PPFY, prevX -= INSTRUMENT_ALITSPEED_PPFY
    ) {
        moveLineTextRight(prevX, currX, val);
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
