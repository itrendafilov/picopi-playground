/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include "displays/displays.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevAirspeed;

void APP_altispeed_zone3_reset() {
    prevAirspeed = -1;
}

typedef enum Instrument_Altispeed_LeftBars {
    INSTRUMENT_ALTISPEED_LEFTBAR_BLACK_MAX = 0,
    INSTRUMENT_ALTISPEED_LEFTBAR_WHITE_BLACK_MAX,
    INSTRUMENT_ALTISPEED_LEFTBAR_GREEN_WHITE_MAX,
    INSTRUMENT_ALTISPEED_LEFTBAR_GREEN_MAX,
    INSTRUMENT_ALTISPEED_LEFTBAR_YELLOW_MAX,
    INSTRUMENT_ALTISPEED_LEFTBAR_COUNT
} Instrument_Altispeed_LeftBars;
static int16_t instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_COUNT];

void APP_altispeed_zone3_precalc(int16_t altispeedParams[]) {
    instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_BLACK_MAX]
             = altispeedParams[APP_INSTRUMENT_ALTISPEED_VMCA] * INSTRUMENT_ALITSPEED_PPK;
    instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_WHITE_BLACK_MAX]
             = altispeedParams[APP_INSTRUMENT_ALTISPEED_VS] * INSTRUMENT_ALITSPEED_PPK;
    instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_GREEN_WHITE_MAX]
             = altispeedParams[APP_INSTRUMENT_ALTISPEED_VFE] * INSTRUMENT_ALITSPEED_PPK;
    instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_GREEN_MAX]
             = altispeedParams[APP_INSTRUMENT_ALTISPEED_VNO] * INSTRUMENT_ALITSPEED_PPK;
    instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_YELLOW_MAX]
             = altispeedParams[APP_INSTRUMENT_ALTISPEED_VNE] * INSTRUMENT_ALITSPEED_PPK;
}

void APP_altispeed_zone3_redraw(int16_t airspeed) {
    Graphics_Rectangle drawZone;

    prevAirspeed = airspeed;
    int fraction = modBy10(airspeed);
    int centerSpeed = divBy10(airspeed);
    fraction >>= 1;

//    int16_t delta = centerSpeed - prevKias;
//    if (0 < delta && delta < 48) {
//        clipping.xMin = 0;
//        clipping.xMax = 9;
//        clipping.yMin =
//        Graphics_setClipRegion(&g_sContext, &clipping);
//    } else if (-48 < delta && delta < 0) {
//        clipping.xMin = 0;
//        clipping.xMax = 9;
//        Graphics_setClipRegion(&g_sContext, &clipping);
//    }

    int16_t absCenter = centerSpeed * INSTRUMENT_ALITSPEED_PPK + 120 + fraction; //+120 pixels in Y for center of screen

    // We utilize the screen clipping baked into the GrLib
    // Draw Red Zone.
    drawZone.xMin = 0;
    drawZone.xMax = 9;
    drawZone.yMax = absCenter - instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_YELLOW_MAX];
    drawZone.yMin = drawZone.yMax - 1000;
    Graphics_setForegroundColor(&g_sContext, ClrRed);
    Graphics_fillRectangle(&g_sContext, &drawZone);


    // Draw Yellow Zone.
    drawZone.yMin = drawZone.yMax;
    drawZone.yMax = absCenter - instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_GREEN_MAX];
    Graphics_setForegroundColor(&g_sContext, ClrYellow);
    Graphics_fillRectangle(&g_sContext, &drawZone);

    // Draw Green Zone.
    drawZone.yMin = drawZone.yMax;
    drawZone.yMax = absCenter - instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_GREEN_WHITE_MAX];
    Graphics_setForegroundColor(&g_sContext, ClrGreen);
    Graphics_fillRectangle(&g_sContext, &drawZone);

    // Draw Green Zone in col 2.
    drawZone.xMin = 5;
    drawZone.xMax = 9;
    drawZone.yMin = drawZone.yMax;
    drawZone.yMax = absCenter - instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_WHITE_BLACK_MAX];
    Graphics_fillRectangle(&g_sContext, &drawZone);

    // Draw White Zone in col 2.
    drawZone.xMin = 0;
    drawZone.xMax = 4;
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_fillRectangle(&g_sContext, &drawZone);

    // Draw White Zone in col 2.
    drawZone.yMin = drawZone.yMax;
    drawZone.yMax = absCenter - instrumentAltispeedLeftBars[INSTRUMENT_ALTISPEED_LEFTBAR_BLACK_MAX];
    Graphics_fillRectangle(&g_sContext, &drawZone);

    // Draw Black Zone in col 1.
    drawZone.xMin = 5;
    drawZone.xMax = 9;
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_fillRectangle(&g_sContext, &drawZone);

    // Draw Black Zone. (in Red ;) )
    drawZone.xMin = 0;
    drawZone.yMin = drawZone.yMax;
    drawZone.yMax = drawZone.yMin + 1000;
    Graphics_setForegroundColor(&g_sContext, ClrRed);
    Graphics_fillRectangle(&g_sContext, &drawZone);

    int16_t delta = 120 + fraction + (centerSpeed % 5) * INSTRUMENT_ALITSPEED_PPK;
    // Color is cached for LineHCustom
    Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // ~0
    delta += 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // -5
    delta += 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // -10
    delta += 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // -15
    delta += 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // -20
    delta -=125; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // +5
    delta -= 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // +10
    delta -= 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // +15
    delta -= 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // +20
    delta -= 25; Graphics_drawLineHCustom(&g_sContext, 0, 9, delta); // +25

    // Restore to no screen clipping
}

APP_DrawPriority APP_altispeed_zone3_check(int16_t airspeed) {
    int delta = abs(airspeed - prevAirspeed);

    if (delta < 2) {  // there is some movement, but it won't change the pixels on screen
        return DRAW_NOTHING;
    } else if (delta < 10) { // very small movement in fraction only
        return COULD_DRAW;
    } else {  // changes are big enough
        return MUST_DRAW;
    }
}
