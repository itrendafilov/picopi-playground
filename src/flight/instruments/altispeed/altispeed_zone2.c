/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <stdint.h>
#include <speedmath.h>
#include "displays/displays.h"
#include "flight/instrument.h"

#include "altispeed_draw.h"

static int16_t prevBaro;

void APP_altispeed_zone2_reset() {
    prevBaro = -1;
}

void APP_altispeed_zone2_redraw(int16_t baro) {
    // ToDo: Min/max baro capping?

    bcd32_t prev = hex2bcd(prevBaro);
    prevBaro = baro;
    bcd32_t curr = hex2bcd(baro);

    if ((curr & 0xF) != (prev & 0xF)) {
        Graphics_drawDigitSmall(175, 37, curr & 0xF);
    }
    curr >>= 4; prev >>= 4;
    if ((curr & 0xF) != (prev & 0xF)) {
        Graphics_drawDigitSmall(163, 37, curr & 0xF);
    }
    curr >>= 4; prev >>= 4;
    if ((curr & 0xF) != (prev & 0xF)) {
        Graphics_drawDigitSmall(146, 37, curr & 0xF);
    }
    curr >>= 4; prev >>= 4;
    if ((curr & 0xF) != (prev & 0xF)) {
        Graphics_drawDigitSmall(134, 37, curr & 0xF);
    }
}

APP_DrawPriority APP_altispeed_zone2_check(int16_t baro) {
    if (baro == prevBaro) {
        return DRAW_NOTHING;
    } else {
        return COULD_DRAW;
    }
}
