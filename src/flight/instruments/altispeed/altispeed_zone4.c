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

static int clipX(int m, int y) {
    // very special on the center
    if (y == 120) return 311;

    if (m) return 310; // m!=0

    if (y < 100) return 301;
    if (y > 140) return 301;

    // center zone
    if (y < 104) return 303;
    if (y < 108) return 306;
    if (y > 136) return 303;
    if (y > 132) return 306;

    return 310;
}

void APP_altispeed_zone4_redraw(int16_t altitude) {

    int currY = INSTRUMENT_ALTISPEED_START_Y(altitude);
    int prevY = INSTRUMENT_ALTISPEED_START_Y(prevAltitude);

    // Move prevY before currY as we don't want to delete what we just printed :D
    if (prevY >= currY + (INSTRUMENT_ALITSPEED_PPFX >> 1)) prevY -= INSTRUMENT_ALITSPEED_PPFX;

    int currM = 0;
    int prevM = 0;

    prevAltitude = altitude;

    Graphics_setFont(&g_sContext, &g_sFontEbrima16b);

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    for (; currY >= 0 || prevY >= 0;
        /* step */
            currY -= INSTRUMENT_ALITSPEED_PPF,
            prevY -= INSTRUMENT_ALITSPEED_PPF,
            currM++, prevM++)
    {
        /* Delete old line */
        if (prevM == INSTRUMENT_ALITSPEED_PPFD) prevM = 0;
        Graphics_drawLineH(&g_sContext, clipX(prevM, prevY), 320, prevY);

        /* Draw new line */
        // Color is cached for LineHCustom
        if (currM == INSTRUMENT_ALITSPEED_PPFD) currM = 0;
        Graphics_drawLineH(&g_sContext, clipX(currM, currY), 320, currY);
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
