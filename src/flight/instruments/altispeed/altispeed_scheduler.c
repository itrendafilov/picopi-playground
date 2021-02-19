/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <stdint.h>
#include "flight/instrument.h"
#include "altispeed_draw.h"
#include "bsp/board.h"

static uint8_t pendingPriorities[DRAW_ZONE_COUNT] = { 0, 0, 0, 0, 0, 0, 0 };
static uint8_t drawTime[DRAW_ZONE_COUNT] = { 0, 0, 0, 0, 0, 0, 0 };

static uint16_t pendingValues[DRAW_INSTRUMENT_COUNT] = { 0, 0, 0, 0 };

void APP_altispeed_reset(APP_DrawInstruments instrument, int16_t value) {
    if (value < 0) value = 0;

    switch (instrument) {
        case DRAW_INSTRUMENT0:
            APP_altispeed_zone0_reset();
            APP_altispeed_zone3_reset();
            APP_altispeed_zone5_reset();
            APP_altispeed_zone0_redraw(value); pendingPriorities[DRAW_ZONE0] = 0;
            APP_altispeed_zone3_redraw(value); pendingPriorities[DRAW_ZONE3] = 0;
            APP_altispeed_zone5_redraw(value); pendingPriorities[DRAW_ZONE5] = 0;
            break;
        case DRAW_INSTRUMENT1:
            APP_altispeed_zone1_reset();
            APP_altispeed_zone4_reset();
            APP_altispeed_zone6a_reset();
            APP_altispeed_zone6b_reset();
            APP_altispeed_zone1_redraw(value); pendingPriorities[DRAW_ZONE1] = 0;
            APP_altispeed_zone4_redraw(value); pendingPriorities[DRAW_ZONE4] = 0;
            APP_altispeed_zone6a_redraw(value); pendingPriorities[DRAW_ZONE6] = 0;
            APP_altispeed_zone6b_redraw(value); pendingPriorities[DRAW_ZONE7] = 0;
            break;
        case DRAW_INSTRUMENT2:
            APP_altispeed_zone2_reset();
            APP_altispeed_zone2_redraw(value); pendingPriorities[DRAW_ZONE2] = 0;
            break;
    }

    pendingValues[instrument] = value;
}

void APP_altispeed_schedule(APP_DrawInstruments instrument, int16_t value) {
    if (value < 0) value = 0;

    switch (instrument) {
        case DRAW_INSTRUMENT0:
            pendingPriorities[DRAW_ZONE0] += APP_altispeed_zone0_check(value) << 2;
            pendingPriorities[DRAW_ZONE3] += APP_altispeed_zone3_check(value) << 1;
            pendingPriorities[DRAW_ZONE5] += APP_altispeed_zone5_check(value);
            break;
        case DRAW_INSTRUMENT1:
            pendingPriorities[DRAW_ZONE1] += APP_altispeed_zone1_check(value) << 2;
            pendingPriorities[DRAW_ZONE4] += APP_altispeed_zone4_check(value) << 1;
            pendingPriorities[DRAW_ZONE6] += APP_altispeed_zone6a_check(value);
            pendingPriorities[DRAW_ZONE7] += APP_altispeed_zone6b_check(value);
            break;
        case DRAW_INSTRUMENT2:
            pendingPriorities[DRAW_ZONE2] += APP_altispeed_zone2_check(value) << 2;
            break;
    }

    pendingValues[instrument] = value;
}

APP_DrawZones APP_altispeed_redraw_next() {
    APP_DrawZones pick = DRAW_ZONE_NONE;
    int i, max = 0;
    for (i = 0; i < DRAW_ZONE_COUNT; i++) {
        if (pendingPriorities[i] > max ||  // higher priority or pick faster if equal
                (pendingPriorities[i] == max && drawTime[pick] > drawTime[i])) {
            pick = (APP_DrawZones)i;
            max = pendingPriorities[i];
        }
    }
    if (pick == DRAW_ZONE_NONE) return pick; // everything is updated

    // ToDo: add log messages for benchmarking?

    uint32_t timeStart = board_millis();

    switch (pick) {
        case DRAW_ZONE0: APP_altispeed_zone0_redraw(pendingValues[DRAW_INSTRUMENT0]); break;
        case DRAW_ZONE3: APP_altispeed_zone3_redraw(pendingValues[DRAW_INSTRUMENT0]); break;
        case DRAW_ZONE5: APP_altispeed_zone5_redraw(pendingValues[DRAW_INSTRUMENT0]); break;

        case DRAW_ZONE1: APP_altispeed_zone1_redraw(pendingValues[DRAW_INSTRUMENT1]); break;
        case DRAW_ZONE4: APP_altispeed_zone4_redraw(pendingValues[DRAW_INSTRUMENT1]); break;
        case DRAW_ZONE6: APP_altispeed_zone6a_redraw(pendingValues[DRAW_INSTRUMENT1]); break;
        case DRAW_ZONE7: APP_altispeed_zone6b_redraw(pendingValues[DRAW_INSTRUMENT1]); break;

        case DRAW_ZONE2: APP_altispeed_zone2_redraw(pendingValues[DRAW_INSTRUMENT2]); break;
    }
    pendingPriorities[pick] = 0;

    uint32_t timeEnd = board_millis();

    // calculate considering clock rewind
    uint8_t executionTime = (timeEnd < timeStart) ? 0xFFFFFFFF - timeStart + timeEnd : timeEnd - timeStart;
    if (drawTime[pick] < executionTime ) {
        drawTime[pick] = executionTime;
    }

//    Log_print2(Diags_USER1, "AS[%d] refresh: %d", pick, executionTime);

    return pick;
}
