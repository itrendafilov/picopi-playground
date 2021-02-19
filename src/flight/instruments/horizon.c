/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <math.h>
#include <stdint.h>
#include "displays/displays.h"
#include "flight/instrument.h"

//#define APP_HORIZON_DEBUG

#ifndef M_PI
#define M_PI    3.1415926535
#endif
#define M_TAU   6.2831853070
#define M_ETA   1.5707963268
#define M_TORAD 0.0174532925

#define ANG_TRI_RADS  0.0434782608 // 2.5 degrees in rads. Used in triangle calculation
#define point_x_on_circle(radius, angle)  (int)round(radius*cos(angle))+160
#define point_y_on_circle(radius, angle)  (int)round(radius*sin(angle))+120

#define sgn(a) ((a < 0) ? -1 : (a > 0))

/* Global Variables */
static float horizonParams[APP_INSTRUMENT_HORIZON_PARAM_COUNT] = {
    /* APP_INSTRUMENT_HORIZON_PITCH */
    0.0,
    /* APP_INSTRUMENT_HORIZON_BANK */
    0.0
};

// A and B. Initialize with values for angle 0
float prevPhi = 0.0;
float prevA = 0.0;
float prevB = 120.0;
int16_t prevY1 = 120;  // at X1=0
int16_t prevY2 = 120;  // at X2=320

static const Graphics_Rectangle rectPlaneL = {
      .xMin = 80,
      .xMax = 120,
      .yMin = 118,
      .yMax = 122
};

static const Graphics_Rectangle rectPlaneC = {
      .xMin = 158,
      .xMax = 162,
      .yMin = 118,
      .yMax = 122
};

static const Graphics_Rectangle rectPlaneR = {
      .xMin = 200,
      .xMax = 240,
      .yMin = 118,
      .yMax = 122
};


void APP_horizon_draw_arc() {
#ifdef APP_HORIZON_DEBUG
    Graphics_setForegroundColor(&g_sContext, ClrGray);
    Graphics_drawCircle(&g_sContext, 160, 120, 105);
    Graphics_drawCircle(&g_sContext, 160, 120, 111);
    Graphics_drawCircle(&g_sContext, 160, 120, 115);

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_drawLineCustom(&g_sContext,   0, 28, 320, 212); // -60 deg
    Graphics_drawLineCustom(&g_sContext,  40,  0, 280, 240); // -45 deg
    Graphics_drawLineCustom(&g_sContext,  91,  0, 229, 240); // -30 deg
    Graphics_drawLineCustom(&g_sContext, 116,  0, 204, 240); // -20 deg
    Graphics_drawLineCustom(&g_sContext, 139,  0, 181, 240); // -10 deg
    Graphics_drawLineCustom(&g_sContext, 160,  0, 160, 240); // 0 deg
    Graphics_drawLineCustom(&g_sContext, 181,  0, 139, 240); // +10 deg
    Graphics_drawLineCustom(&g_sContext, 204,  0, 116, 240); // +20 deg
    Graphics_drawLineCustom(&g_sContext, 229,  0,  91, 240); // +30 deg
    Graphics_drawLineCustom(&g_sContext, 280,  0,  40, 240); // +45 deg
    Graphics_drawLineCustom(&g_sContext,   0,212, 320,  28); // +60 deg
#endif

    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawLineCustom(&g_sContext,  60, 63,  69, 68); // -60 deg
    Graphics_drawLineCustom(&g_sContext,  82, 42,  86, 46); // -45 deg
    Graphics_drawLineCustom(&g_sContext, 103, 20, 108, 29); // -30 deg
    Graphics_drawLineCustom(&g_sContext, 122, 16, 124, 21); // -20 deg
    Graphics_drawLineCustom(&g_sContext, 141, 11, 142, 17); // -10 deg
    Graphics_fillTriangle(&g_sContext, 160, 15, 155, 5, 165, 5); // 0 deg
    Graphics_drawLineCustom(&g_sContext, 179, 11, 178, 17); // +10 deg
    Graphics_drawLineCustom(&g_sContext, 198, 16, 196, 21); // +20 deg
    Graphics_drawLineCustom(&g_sContext, 218, 20, 213, 29); // +30 deg
    Graphics_drawLineCustom(&g_sContext, 238, 42, 234, 46); // +45 deg
    Graphics_drawLineCustom(&g_sContext, 260, 63, 251, 67); // +60 deg
}

int val_x1, val_y1, val_x2, val_y2, val_x3, val_y3;
void APP_horizon_draw_bank() {
    Graphics_setForegroundColor(&g_sContext, ClrGold);
    float angle = prevPhi;
    angle -= M_ETA; // rotate 90 degrees
//    int x0 = 93.0*cos(angle)+160, y0 = 93.0*sin(angle)+120; // for clear
    val_x1 = point_x_on_circle(103.0, angle);
    val_y1 = point_y_on_circle(103.0, angle);
    angle += ANG_TRI_RADS;
    val_x2 = point_x_on_circle(93.0, angle);
    val_y2 = point_y_on_circle(93.0, angle);
    angle -= ANG_TRI_RADS; angle -= ANG_TRI_RADS;
    val_x3 = point_x_on_circle(93.0, angle);
    val_y3 = point_y_on_circle(93.0, angle);
    Graphics_fillTriangle(&g_sContext, val_x1, val_y1, val_x2, val_y2, val_x3, val_y3);
}

void APP_horizon_clear_bank() {
    //ToDo: proper background calculation
    Graphics_setForegroundColor(&g_sContext, ClrBlue);
    Graphics_fillTriangle(&g_sContext, val_x1, val_y1, val_x2, val_y2, val_x3, val_y3);
}


void APP_horizon_draw_plane() {
    Graphics_setForegroundColor(&g_sContext, ClrGold);
    Graphics_fillRectangle(&g_sContext, &rectPlaneL);
    Graphics_fillRectangle(&g_sContext, &rectPlaneC);
    Graphics_fillRectangle(&g_sContext, &rectPlaneR);
}

void APP_horizon_draw_static() {
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawLineH(&g_sContext, 140, 180,  40); // -10
    Graphics_drawLineH(&g_sContext, 155, 165,  60); // -7.5
    Graphics_drawLineH(&g_sContext, 150, 170,  80); // -5
    Graphics_drawLineH(&g_sContext, 155, 165, 100); // -2.5
    Graphics_drawLineH(&g_sContext, 140, 180, 120); // center 0
    Graphics_drawLineH(&g_sContext, 155, 165, 140); // +2.5
    Graphics_drawLineH(&g_sContext, 150, 170, 160); // +5
    Graphics_drawLineH(&g_sContext, 155, 165, 180); // +7.5
    Graphics_drawLineH(&g_sContext, 140, 180, 200); // +10

    Graphics_setForegroundColor(&g_sContext, ClrGold);
    APP_horizon_draw_plane();

    Graphics_setForegroundColor(&g_sContext, ClrWhite); // restore default
}

void APP_horizon_init() {
    // Keep in mind we draw in reversed X-Y for Landscape LCD
    Graphics_Rectangle rect = {
          .xMin = 0, .xMax = LCD_Y_SIZE,
          .yMin = 0, .yMax = LCD_X_SIZE / 2
    };
    Graphics_setForegroundColor(&g_sContext, ClrBlue);
    Graphics_fillRectangle(&g_sContext, &rect);

    rect.yMin = rect.yMax;
    rect.yMax = LCD_X_SIZE;

    Graphics_setForegroundColor(&g_sContext, ClrSaddleBrown);
    Graphics_fillRectangle(&g_sContext, &rect);

    APP_horizon_draw_static();
    APP_horizon_draw_arc();

#ifdef APP_HORIZON_DEBUG
    // test custom line-draw method
    float rotate = M_PI / 12, angle = 0;
    while (angle < M_TAU)
    {
        Graphics_drawLineAtAngle(&g_sContext, angle, 160, 120, 80, 40);
        angle += rotate;
    }
#endif

    Graphics_setForegroundColor(&g_sContext, ClrWhite); // restore
}

volatile bool drawing = false;

void APP_horizon_refresh() {
    drawing = true;

    float phi = horizonParams[APP_INSTRUMENT_HORIZON_BANK] * M_TORAD; // convert to rads and store
    float currA = tan(phi);
    // we draw attack angle with 8px per degree
    float currB = 120.0 - 160.0*currA + horizonParams[APP_INSTRUMENT_HORIZON_PITCH]*8.0;

    int16_t currY1 = currB; // at X1=0
    int16_t currY2 = currA * 320.0 + currB; // at X2=320

    // ToDo: handle negative phi
    // ToDo: handle special cases = phi 0 and 90.

#ifdef APP_HORIZON_DEBUG
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawLine(&g_sContext, 0, currY1, 320, currY2);

    __no_operation(); // for debugger
#endif

//    if (currY1 < prevY1) {
//        Graphics_setForegroundColor(&g_sContext, ClrSaddleBrown);
//        Graphics_fillTriangle(&g_sContext, 0, currY1, 0, prevY1, 160, 120);
//        Graphics_setForegroundColor(&g_sContext, ClrBlue);
//        Graphics_fillTriangle(&g_sContext, 320, prevY2, 320, currY2, 160, 120);
//    } else {
//        Graphics_setForegroundColor(&g_sContext, ClrBlue);
//        Graphics_fillTriangle(&g_sContext, 0, prevY1, 0, currY1, 160, 120);
//        Graphics_setForegroundColor(&g_sContext, ClrSaddleBrown);
//        Graphics_fillTriangle(&g_sContext, 320, currY2, 320, prevY2, 160, 120);
//    }
//
    prevY1 = currY1;
    prevY2 = currY2;

    // ToDo: cache previous into memory?
    int x, currY, prevY;
    for (x = 0; x < 320; x++) {
        prevY = (prevA*(float)x + prevB);
        currY = (currA*(float)x + currB);
        if (prevY == currY) {
            // best case.. nothing to redraw
        } else if (prevY < currY) {
            Graphics_setForegroundColor(&g_sContext, ClrBlue);
            Graphics_drawLineV(&g_sContext, x, currY, prevY);
        } else {
            Graphics_setForegroundColor(&g_sContext, ClrSaddleBrown);
            Graphics_drawLineV(&g_sContext, x, prevY, currY);
        }
    }


    // Remove old arrow
    APP_horizon_clear_bank();

    prevPhi = phi;
    prevA = currA;
    prevB = currB;

    // Draw new bank arrow
    APP_horizon_draw_bank();

    // Redraw static content
    APP_horizon_draw_static();
    APP_horizon_draw_arc();

    drawing = false;
}

float stepPitch = 1.0;
float stepBank = 1.0;

void APP_horizon_input(APP_input_events event) {
    switch (event) {
        case APP_INPUT_ENCODER1_CCW:
            APP_horizon_update(APP_INSTRUMENT_HORIZON_PITCH, horizonParams[APP_INSTRUMENT_HORIZON_PITCH] - stepPitch);
            break;
        case APP_INPUT_ENCODER1_CW:
            APP_horizon_update(APP_INSTRUMENT_HORIZON_PITCH, horizonParams[APP_INSTRUMENT_HORIZON_PITCH] + stepPitch);
            break;
        case APP_INPUT_ENCODER2_CCW:
            APP_horizon_update(APP_INSTRUMENT_HORIZON_BANK, horizonParams[APP_INSTRUMENT_HORIZON_BANK] - stepBank);
            break;
        case APP_INPUT_ENCODER2_CW:
            APP_horizon_update(APP_INSTRUMENT_HORIZON_BANK, horizonParams[APP_INSTRUMENT_HORIZON_BANK] + stepBank);
            break;

        case APP_INPUT_BUTTON0:
            stepBank = 0.1;
            stepPitch = 0.1;
            break;
        case APP_INPUT_BUTTON1:
            stepBank = 0.5;
            stepPitch = 0.5;
            break;
        case APP_INPUT_BUTTON2:
            stepBank = 1.0;
            stepPitch = 1.0;
            break;
        case APP_INPUT_BUTTON3:
            stepBank = 5.0;
            stepPitch = 5.0;
            break;

        case APP_INPUT_BUTTON4:
            APP_instruments_change(APP_INSTRUMENT_RADIO);
            break;
        case APP_INPUT_BUTTON5:
             APP_instruments_change(APP_INSTRUMENT_ALTITUDE_SPEED);
             break;
    }
}

void APP_horizon_update(APP_instrument_horizon_param param, float value) {
    if (!drawing) {
        horizonParams[param] = value;
        APP_instruments_refresh();
    }
}
