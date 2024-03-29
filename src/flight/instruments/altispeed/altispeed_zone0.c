/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <speedmath.h>
#include <stdint.h>
#include "displays/displays.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevAirspeed;
void APP_altispeed_zone0_reset() {
    prevAirspeed = -1;
}

void APP_altispeed_zone0_redraw(int16_t airspeed) {

    if (airspeed < 0) {
        airspeed = 0;
    }

    Graphics_setForegroundColor(&g_sContext, ClrWhite);

    // restore previous value and replace with current
    bcd32_t prev = prevAirspeed == -1 ? 0xFFFFFFF0 : hex2bcd(prevAirspeed);
    bcd32_t curr = hex2bcd(airspeed);
    prevAirspeed = airspeed;

    // Draw rightmost digit first
    int8_t currRoll = (int8_t)curr & 0xF; // get last BCD as fraction
    int8_t prevRoll = (int8_t)prev & 0xF; // get last BCD
    curr >>= 4; // move to next BCD
    prev >>= 4;
    int8_t currDigit = (int8_t)curr & 0xF;
    int8_t prevDigit = (int8_t)prev & 0xF;
#ifdef INSTRUMENT_ALITSPEED_USE_MEMMAPED    
    Graphics_drawDigitMediumMemmapRoll(150, 65, currDigit, currRoll);
#else
    Graphics_drawDigitMediumRoll(150, 65, currDigit, currRoll);
#endif
    int y;
    int8_t currLastDigit;
    for (y = 45; y >= 25; y-=20) {
        currLastDigit = currDigit;
        curr >>= 4; // move to next BCD currDigit
        prev >>= 4;
        currDigit = curr & 0xF;
        prevDigit = prev & 0xF;

        if (currLastDigit == 9 && currRoll >= 3) {
#ifdef INSTRUMENT_ALITSPEED_USE_MEMMAPED                
            Graphics_drawDigitMediumMemmapRollSmall(150, y, currDigit, currRoll);
#else
            Graphics_drawDigitMediumRollSmall(150, y, currDigit, currRoll);
#endif
        } else if (curr) {  // has more digits
            currRoll = 0; // no need to roll digits on left
            if (prevRoll) {
                Graphics_setForegroundColor(&g_sContext, ClrBlack); // clear anything stale in the roll
                Graphics_clearDigitMediumRollSmall(150, y);
                Graphics_setForegroundColor(&g_sContext, ClrWhite);
            }
            if (currDigit != prevDigit || (prevRoll >= 3)) {
#ifdef INSTRUMENT_ALITSPEED_USE_MEMMAPED                
                Graphics_drawDigitMediumMemmap(150, y, currDigit);
#else
                Graphics_drawDigitMedium(150, y, currDigit);
#endif
            }
        } else {
            Graphics_setForegroundColor(&g_sContext, ClrBlack);
            Graphics_clearDigitMedium(150, y); // clear anything stale
            if (prevRoll) {                    // clear anything stale in the roll
                Graphics_clearDigitMediumRollSmall(150, y);
            }
            Graphics_setForegroundColor(&g_sContext, ClrWhite);
        }
    }
}

APP_DrawPriority APP_altispeed_zone0_check(int16_t airspeed) {
    int16_t delta = airspeed - prevAirspeed;

    if (!delta) {
        return DRAW_NOTHING;
    } else if (abs(delta) < 2) { // only in part of a fraction
        return COULD_DRAW;
    } else {  // changes are big enough
        return MUST_DRAW;
    }
}
