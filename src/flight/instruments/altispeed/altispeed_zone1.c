/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include <speedmath.h>
#include "displays/displays.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevAltitude;

void APP_altispeed_zone1_reset() {
    prevAltitude = -1;
}

void APP_altispeed_zone1_redraw(int16_t altitude) {

    if (altitude < 0) {
        altitude = 0;
    }

    Graphics_setForegroundColor(&g_sContext, ClrWhite);

    // restore previous value and replace with current
    bcd32_t prev = prevAltitude == -1 ? 0xFFFFFFF0 : hex2bcd(prevAltitude);
    bcd32_t curr = hex2bcd(altitude);
    prevAltitude = altitude;

    // Draw rightmost digit first
    int8_t currRoll = (int8_t)curr & 0xF; // get last BCD as fraction
    int8_t prevRoll = (int8_t)prev & 0xF; // get last BCD
    curr >>= 4; // move to next BCD
    prev >>= 4;
    int8_t currDigit = (int8_t)curr & 0xF;
    int8_t prevDigit = (int8_t)prev & 0xF;
    Graphics_drawDigitMediumRollZeros(110, 280, currRoll);
    Graphics_drawDigitMediumMemmapRoll(110, 262, currDigit, currRoll);

    int y;
    int8_t currLastDigit;
    for (y = 244; y >= 208; y-=18) {
        currLastDigit = currDigit;
        curr >>= 4; // move to next BCD currDigit
        prev >>= 4;
        currDigit = curr & 0xF;
        prevDigit = prev & 0xF;

        if (currLastDigit == 9 && currRoll >= 3) {
            Graphics_drawDigitMediumMemmapRollSmall(110, y, currDigit, currRoll - 3); // -3 so we stay in sync with other digits
        } else if (curr) {  // has more digits
            currRoll = 0; // no need to roll digits on left
            if (prevRoll) {
                Graphics_setForegroundColor(&g_sContext, ClrBlack); // clear anything stale in the roll
                Graphics_clearDigitMediumRollSmall(110, y);
                Graphics_setForegroundColor(&g_sContext, ClrWhite);
            }
            if (currDigit != prevDigit || (prevRoll >= 3)) {
                Graphics_drawDigitMediumMemmap(110, y, currDigit);
            }
        } else {
            Graphics_setForegroundColor(&g_sContext, ClrBlack);
            Graphics_clearDigitMedium(110, y); // clear anything stale
            if (prevRoll) {                    // clear anything stale in the roll
                Graphics_clearDigitMediumRollSmall(110, y);
            }
            Graphics_setForegroundColor(&g_sContext, ClrWhite);
        }
    }
}


APP_DrawPriority APP_altispeed_zone1_check(int16_t altitude) {
    int delta = altitude - prevAltitude;

    if (!delta) {
        return DRAW_NOTHING;
    } else if (abs(delta) < 20) { // very small movement
        return COULD_DRAW;
    } else {
        return MUST_DRAW;
    }
}
