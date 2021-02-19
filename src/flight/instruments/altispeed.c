/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include "displays/displays.h"
#include "displays/drivers/vd_driver.h"
#include "flight/instrument.h"

#include "flight/instruments/altispeed/altispeed_draw.h"

/* Global Variables */
static int16_t altispeedParams[APP_INSTRUMENT_ALTISPEED_PARAM_COUNT] = {
    /* APP_INSTRUMENT_ALTISPEED_BARO */
    2992,
    /* APP_INSTRUMENT_ALTISPEED_VNE == red line start == yellow line end */
    163,
    /* APP_INSTRUMENT_ALTISPEED_VNO == yellow line start == green line end */
    129,
    /* APP_INSTRUMENT_ALTISPEED_VFE == white line end*/
    85,
    /* APP_INSTRUMENT_ALTISPEED_VS == green line start */
    48,
    /* APP_INSTRUMENT_ALTISPEED_VMCA == white line start */
    40,
    /* APP_INSTRUMENT_ALTISPEED_ALTITUDE == current altitude reading */
    5000,
    /* APP_INSTRUMENT_ALTISPEED_KIAS == current airspeed reading */
    100 * INSTRUMENT_ALITSPEED_FACTOR_KIAS
};

void APP_altispeed_init() {
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_setBackgroundColor(&g_sContext, ClrBlack);
    Graphics_clearDisplay(&g_sContext);

    // We draw a lot of horizontal lines, thus prepare a buffer with that color
    // N.B. it is also used in zone redraw methods, so don't fill with other color
//    Graphics_loadCustomLineColor(&g_sContext);

    //                    _
    //               ____| |
    // Left arrow   <____  |
    //                   |_|
    //
    Graphics_drawLineCustom(&g_sContext, 10, 120, 20, 100);
    Graphics_drawLineCustom(&g_sContext, 10, 120, 20, 140);
    Graphics_drawLineHCustom(&g_sContext, 20, 60, 100);
    Graphics_drawLineVCustom(&g_sContext, 60, 90, 100);
    Graphics_drawLineHCustom(&g_sContext, 60, 85, 90);
    Graphics_drawLineVCustom(&g_sContext, 85, 90, 150);
    Graphics_drawLineHCustom(&g_sContext, 60, 85, 150);
    Graphics_drawLineVCustom(&g_sContext, 60, 140, 150);
    Graphics_drawLineHCustom(&g_sContext, 20, 60, 140);

    //                    __
    //                ___|  |
    // Right arrow   |___    >
    //                   |__|
    //
    Graphics_drawLineCustom(&g_sContext, 309, 120, 300, 100);
    Graphics_drawLineCustom(&g_sContext, 309, 120, 300, 140);
    Graphics_drawLineVCustom(&g_sContext, 300, 90, 100);
    Graphics_drawLineHCustom(&g_sContext, 250, 300, 90);
    Graphics_drawLineVCustom(&g_sContext, 250, 90, 100);
    Graphics_drawLineHCustom(&g_sContext, 200, 250, 100);
    Graphics_drawLineVCustom(&g_sContext, 200, 100, 140);
    Graphics_drawLineHCustom(&g_sContext, 200, 250, 140);
    Graphics_drawLineVCustom(&g_sContext, 250, 140, 150);
    Graphics_drawLineHCustom(&g_sContext, 250, 300, 150);
    Graphics_drawLineVCustom(&g_sContext, 300, 140, 150);

    // Baro box
    Graphics_drawLineHCustom(&g_sContext, 130, 190, 30);
    Graphics_drawLineHCustom(&g_sContext, 130, 190, 60);
    Graphics_drawLineVCustom(&g_sContext, 130, 30, 60);
    Graphics_drawLineVCustom(&g_sContext, 190, 30, 60);
    Graphics_setFont(&g_sContext, &g_sFontEbrima24b);
    Graphics_drawStringCentered(&g_sContext, "BARO", 4, 160, 15, false);

    const Graphics_Rectangle drawBaroDot = {
           .xMin = 159,
           .xMax = 161,
           .yMin = 49,
           .yMax = 51
    };
    Graphics_fillRectangle(&g_sContext, (Graphics_Rectangle*)&drawBaroDot);


    // Initialize the Virtual display to cache the '0' we print a lot ;D
    Graphics_initContext(&g_vdContext, &g_vdDisplay);

//  Preapare only when in RW mode.
//    Graphics_setFont(&g_vdContext, &g_sFontEbrima16b);
//    Graphics_setForegroundColor(&g_vdContext, ClrWhite);
//    Graphics_setBackgroundColor(&g_vdContext, ClrBlack);
//    Graphics_clearDisplay(&g_vdContext);
//    Graphics_drawString(&g_vdContext, "0", 1, x, -1, false); x += 8;

    // pre-calculate left bar absolute positions
    APP_altispeed_zone3_precalc(altispeedParams);

    // Initialize scheduler to draw initial values
    APP_altispeed_reset(DRAW_INSTRUMENT0, altispeedParams[APP_INSTRUMENT_ALTISPEED_KIAS]);
    APP_altispeed_reset(DRAW_INSTRUMENT1, altispeedParams[APP_INSTRUMENT_ALTISPEED_ALTITUDE]);
    APP_altispeed_reset(DRAW_INSTRUMENT2, altispeedParams[APP_INSTRUMENT_ALTISPEED_BARO]);

    // do screen redraw
    APP_altispeed_refresh();
}

void APP_altispeed_refresh() {
    APP_DrawZones zone = APP_altispeed_redraw_next();
    // update again until nothing is left
    if (zone != DRAW_ZONE_NONE) APP_instruments_refresh();
}

void APP_altispeed_input(APP_input_events event) {
    switch (event) {
        case APP_INPUT_ENCODER1_CCW:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_KIAS, altispeedParams[APP_INSTRUMENT_ALTISPEED_KIAS] - 1);  // 10 = 1 * INSTRUMENT_ALITSPEED_FACTOR_KIAS
            break;
        case APP_INPUT_ENCODER1_CW:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_KIAS, altispeedParams[APP_INSTRUMENT_ALTISPEED_KIAS] + 1);  // 10 = 1 * INSTRUMENT_ALITSPEED_FACTOR_KIAS
            break;
        case APP_INPUT_ENCODER2_CCW:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_BARO, altispeedParams[APP_INSTRUMENT_ALTISPEED_BARO] - 1);
            break;
        case APP_INPUT_ENCODER2_CW:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_BARO, altispeedParams[APP_INSTRUMENT_ALTISPEED_BARO] + 1);
            break;

        case APP_INPUT_BUTTON0:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_ALTITUDE, altispeedParams[APP_INSTRUMENT_ALTISPEED_ALTITUDE] + 500);
            break;
        case APP_INPUT_BUTTON1:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_ALTITUDE, altispeedParams[APP_INSTRUMENT_ALTISPEED_ALTITUDE] + 25);
            break;
        case APP_INPUT_BUTTON2:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_ALTITUDE, altispeedParams[APP_INSTRUMENT_ALTISPEED_ALTITUDE] - 25);
            break;
        case APP_INPUT_BUTTON3:
            APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_ALTITUDE, altispeedParams[APP_INSTRUMENT_ALTISPEED_ALTITUDE] - 500);
            break;

        case APP_INPUT_BUTTON4:
            APP_instruments_change(APP_INSTRUMENT_HORIZON);
            break;
        case APP_INPUT_BUTTON5:
             APP_instruments_change(APP_INSTRUMENT_RADIO);
             break;
    }
}

void APP_altispeed_update(APP_instrument_altispeed_param param, int16_t value) {
    if (param == APP_INSTRUMENT_ALTISPEED_KIAS) {
        APP_altispeed_schedule(DRAW_INSTRUMENT0, value);
    } else if (param == APP_INSTRUMENT_ALTISPEED_ALTITUDE) {
        APP_altispeed_schedule(DRAW_INSTRUMENT1, value);
    } else if (param == APP_INSTRUMENT_ALTISPEED_BARO) {
        APP_altispeed_schedule(DRAW_INSTRUMENT2, value);
    } else {
        APP_altispeed_zone3_precalc(altispeedParams);
    }
    altispeedParams[param] = value;

    APP_instruments_refresh();
}
