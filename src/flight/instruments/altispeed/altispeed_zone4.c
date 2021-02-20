/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include "displays/displays.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevAltitude;

void APP_altispeed_zone4_reset() {
    prevAltitude = -1;
}

static int clipY(int m, int x) {
    // very special on the center
    if (x == 120) return 311;

    if (m) return 310; // m!=0

    if (x < 100) return 301;
    if (x > 140) return 301;

    // center zone
    if (x < 104) return 303;
    if (x < 108) return 306;
    if (x > 136) return 303;
    if (x > 132) return 306;

    return 310;
}

void APP_altispeed_zone4_redraw(int16_t altitude) {

    int currX = INSTRUMENT_ALTISPEED_START_X(altitude);
    int prevX = INSTRUMENT_ALTISPEED_START_X(prevAltitude);

    // Move prevX before currX as we don't want to delete what we just printed :D
    if (prevX >= currX + (INSTRUMENT_ALITSPEED_PPFY >> 1)) prevX -= INSTRUMENT_ALITSPEED_PPFY;

    int currM = 0;
    int prevM = 0;

    prevAltitude = altitude;

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);

    for (; currX >= 0 || prevX >= 0;
        /* step */
            currX -= INSTRUMENT_ALITSPEED_PPF,
            prevX -= INSTRUMENT_ALITSPEED_PPF,
            currM++, prevM++)
    {
        /* Delete old line */
        Graphics_setForegroundColor(&g_sContext, ClrBlack);
        if (prevM == INSTRUMENT_ALITSPEED_PPFD) prevM = 0;
        Graphics_drawLineV(&g_sContext, prevX, clipY(prevM, prevX), 320);

        /* Draw new line */
        Graphics_setForegroundColor(&g_sContext, ClrWhite);
        if (currM == INSTRUMENT_ALITSPEED_PPFD) currM = 0;
        Graphics_drawLineV(&g_sContext, currX, clipY(currM, currX), 320);
    }
}


APP_DrawPriority APP_altispeed_zone4_check(int16_t altitude) {
    int delta = abs(altitude - prevAltitude);

    // check if there is some movement, but it won't change the pixels on screen
    if (delta < INSTRUMENT_ALTISPEED_APPD(1)) {
        return DRAW_NOTHING;
    } else if (delta < 5 * INSTRUMENT_ALTISPEED_APPD(1)) { // very small movement
        return COULD_DRAW;
    } else {
        return MUST_DRAW;
    }
}
