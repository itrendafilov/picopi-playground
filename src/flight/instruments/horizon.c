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
#define point_x_on_circle(radius, angle)  (int)round(radius*sin(angle))+160
#define point_y_on_circle(radius, angle)  (int)round(radius*cos(angle))+240

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
float prevB = 160.0;
int16_t prevX1 = 160;  // at y1=0
int16_t prevX2 = 160;  // at y2=480

static const Graphics_Rectangle rectPlaneL = {
      .xMin = 158,
      .xMax = 162,
      .yMin = 120,
      .yMax = 180
};

static const Graphics_Rectangle rectPlaneC = {
      .xMin = 158,
      .xMax = 162,
      .yMin = 238,
      .yMax = 242
};

static const Graphics_Rectangle rectPlaneR = {
      .xMin = 158,
      .xMax = 162,
      .yMin = 300,
      .yMax = 360
};


void APP_horizon_draw_arc() {
#ifdef APP_HORIZON_DEBUG
    Graphics_setForegroundColor(&g_sContext, ClrGray);
    Graphics_drawCircle(&g_sContext, 160, 240, 140);
    Graphics_drawCircle(&g_sContext, 160, 240, 148);
    Graphics_drawCircle(&g_sContext, 160, 240, 156);

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_drawLineCustom(&g_sContext,   0, 56, 320, 424); // -60 deg
    Graphics_drawLineCustom(&g_sContext,  40,  0, 280, 480); // -45 deg
    Graphics_drawLineCustom(&g_sContext,  91,  0, 229, 480); // -30 deg
    Graphics_drawLineCustom(&g_sContext, 116,  0, 204, 480); // -20 deg
    Graphics_drawLineCustom(&g_sContext, 139,  0, 181, 480); // -10 deg
    Graphics_drawLineCustom(&g_sContext, 160,  0, 160, 480); // 0 deg
    Graphics_drawLineCustom(&g_sContext, 181,  0, 139, 480); // +10 deg
    Graphics_drawLineCustom(&g_sContext, 204,  0, 116, 480); // +20 deg
    Graphics_drawLineCustom(&g_sContext, 229,  0,  91, 480); // +30 deg
    Graphics_drawLineCustom(&g_sContext, 280,  0,  40, 480); // +45 deg
    Graphics_drawLineCustom(&g_sContext,   0,424, 320,  56); // +60 deg
#endif

    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawLineCustom(&g_sContext,  82, 105, 90, 119); // -60 deg
    Graphics_drawLineCustom(&g_sContext,  55, 135, 61, 141); // -45 deg
    Graphics_drawLineCustom(&g_sContext, 25, 162, 39, 170); // -30 deg
    Graphics_drawLineCustom(&g_sContext, 21, 189, 28, 192); // -20 deg
    Graphics_drawLineCustom(&g_sContext, 14, 214, 22, 216); // -10 deg
    Graphics_fillTriangle(&g_sContext, 20, 240, 4, 235, 4, 245); // 0 deg
    Graphics_drawLineCustom(&g_sContext, 14, 266, 22, 264); // +10 deg
    Graphics_drawLineCustom(&g_sContext, 21, 291, 28, 288); // +20 deg
    Graphics_drawLineCustom(&g_sContext, 25, 318, 39, 310); // +30 deg
    Graphics_drawLineCustom(&g_sContext, 55, 345, 61, 339); // +45 deg
    Graphics_drawLineCustom(&g_sContext, 82, 375, 90, 361); // +60 deg
}

int val_x1, val_y1, val_x2, val_y2, val_x3, val_y3;
void APP_horizon_draw_bank() {
    Graphics_setForegroundColor(&g_sContext, ClrGold);
    float angle = prevPhi;
    angle -= M_ETA; // rotate 90 degrees
//    int x0 = 93.0*cos(angle)+160, y0 = 93.0*sin(angle)+120; // for clear
    val_x1 = point_x_on_circle(138.0, angle);
    val_y1 = point_y_on_circle(138.0, angle);
    angle += ANG_TRI_RADS;
    val_x2 = point_x_on_circle(128.0, angle);
    val_y2 = point_y_on_circle(128.0, angle);
    angle -= ANG_TRI_RADS; angle -= ANG_TRI_RADS;
    val_x3 = point_x_on_circle(128.0, angle);
    val_y3 = point_y_on_circle(128.0, angle);
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
    Graphics_drawLineV(&g_sContext,  40, 210, 270); // -10
    Graphics_drawLineV(&g_sContext,  70, 232, 248); // -7.5
    Graphics_drawLineV(&g_sContext, 100, 225, 255); // -5
    Graphics_drawLineV(&g_sContext, 130, 232, 248); // -2.5
    Graphics_drawLineV(&g_sContext, 160, 210, 270); // center 0
    Graphics_drawLineV(&g_sContext, 190, 232, 248); // +2.5
    Graphics_drawLineV(&g_sContext, 220, 225, 255); // +5
    Graphics_drawLineV(&g_sContext, 250, 232, 248); // +7.5
    Graphics_drawLineV(&g_sContext, 280, 210, 270); // +10

    Graphics_setForegroundColor(&g_sContext, ClrGold);
    APP_horizon_draw_plane();

    Graphics_setForegroundColor(&g_sContext, ClrWhite); // restore default
}

void APP_horizon_init() {
    Graphics_Rectangle rect = {
          .xMin = 0, .xMax = LCD_X_SIZE / 2,
          .yMin = 0, .yMax = LCD_Y_SIZE - 1
    };
    Graphics_setForegroundColor(&g_sContext, ClrBlue);
    Graphics_fillRectangle(&g_sContext, &rect);

    rect.xMin = rect.xMax;
    rect.xMax = LCD_X_SIZE - 1;

    Graphics_setForegroundColor(&g_sContext, ClrSaddleBrown);
    Graphics_fillRectangle(&g_sContext, &rect);

    APP_horizon_draw_static();
    APP_horizon_draw_arc();

#ifdef APP_HORIZON_DEBUG
    // test custom line-draw method
    float rotate = M_PI / 12, angle = 0;
    while (angle < M_TAU)
    {
        Graphics_drawLineAtAngle(&g_sContext, angle, 160, 240, 160, 40);
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
    // we draw attack angle with 12px per degree
    float currB = 160.0 - 240.0*currA + horizonParams[APP_INSTRUMENT_HORIZON_PITCH]*12.0;

    int16_t currX1 = currB; // at y1=0
    int16_t currX2 = currA * 480.0 + currB; // at y2=480

    // ToDo: handle negative phi
    // ToDo: handle special cases = phi 0 and 90.

#ifdef APP_HORIZON_DEBUG
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_drawLine(&g_sContext, 0, currX1, 320, currX2);
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
    prevX1 = currX1;
    prevX2 = currX2;

    // ToDo: cache previous into memory?
    int y, currX, prevX;
    for (y = 0; y < 480; y++) {
        prevX = (prevA*(float)y + prevB);
        currX = (currA*(float)y + currB);
        if (prevX == currX) {
            // best case.. nothing to redraw
        } else if (prevX < currX) {
            Graphics_setForegroundColor(&g_sContext, ClrBlue);
            Graphics_drawLineH(&g_sContext, currX, prevX, y);
        } else {
            Graphics_setForegroundColor(&g_sContext, ClrSaddleBrown);
            Graphics_drawLineH(&g_sContext, prevX, currX, y);
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
