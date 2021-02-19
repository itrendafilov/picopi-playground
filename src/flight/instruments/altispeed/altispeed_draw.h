/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef SRC_FLIGHT_INSTRUMENTS_ALTISPEED_DRAW_H_
#define SRC_FLIGHT_INSTRUMENTS_ALTISPEED_DRAW_H_

#define INSTRUMENT_ALITSPEED_PPK             5   // PIXELS_PER_KNOT
#define INSTRUMENT_ALITSPEED_PPFD            5   // Sub Divisions between Major
#define INSTRUMENT_ALITSPEED_PPF             16  // PIXELS_PER_100_FEET
#define INSTRUMENT_ALITSPEED_PPFX            80  // PIXELS_PER_500_FEET
#define INSTRUMENT_ALITSPEED_FACTOR_KIAS     10  // Factor with which we multiply the KIAS so we don't use float

#define INSTRUMENT_ALTISPEED_APPD(alt)       ((alt << 2) / 25)        // divide by 6.25 (int optimized) = (100 / INSTRUMENT_ALITSPEED_PPF)
#define INSTRUMENT_ALTISPEED_ATOX(alt)       (INSTRUMENT_ALTISPEED_APPD(alt % 100))
#define INSTRUMENT_ALTISPEED_ATOM(alt)       ((alt / 100) % 5)
#define INSTRUMENT_ALTISPEED_ATOD(alt)       (alt / 500)

/*
 * 1. Start at Y 120
 * 2. Add the delta between last minor division and current value
 * 3. Add delta from minor to major division
 * 4. We can view up to two major scale so add an offset for them
 */
#define INSTRUMENT_ALTISPEED_START_YC(alt)   (120 + INSTRUMENT_ALTISPEED_ATOX(alt) + INSTRUMENT_ALTISPEED_ATOM(alt) * INSTRUMENT_ALITSPEED_PPF)
#define INSTRUMENT_ALTISPEED_START_Y(alt)    (INSTRUMENT_ALTISPEED_START_YC(alt) + 2 * INSTRUMENT_ALITSPEED_PPFX)


typedef enum {
    DRAW_NOTHING = 0,
    COULD_DRAW = 1,
    SHOULD_DRAW = 2,
    MUST_DRAW = 4,
} APP_DrawPriority;

typedef enum {
    DRAW_ZONE0 = 0,
    DRAW_ZONE1,
    DRAW_ZONE2,
    DRAW_ZONE3,
    DRAW_ZONE4,
    DRAW_ZONE5,
    DRAW_ZONE6,
    DRAW_ZONE7,
    DRAW_ZONE_COUNT,
    DRAW_ZONE_NONE
} APP_DrawZones;

typedef enum {
    DRAW_INSTRUMENT0 = 0,
    DRAW_INSTRUMENT1,
    DRAW_INSTRUMENT2,
    DRAW_INSTRUMENT3,
    DRAW_INSTRUMENT_COUNT
} APP_DrawInstruments;


/* Zoned scheduler */
void APP_altispeed_schedule(APP_DrawInstruments instrument, int16_t value);
APP_DrawZones APP_altispeed_redraw_next();
void APP_altispeed_reset(APP_DrawInstruments instrument, int16_t value);

/* Barometer zones */
void APP_altispeed_zone2_redraw(int16_t baro);
APP_DrawPriority APP_altispeed_zone2_check(int16_t baro);
void APP_altispeed_zone2_reset();

/* Airspeed zones */
void APP_altispeed_zone0_redraw(int16_t airspeed);
APP_DrawPriority APP_altispeed_zone0_check(int16_t airspeed);
void APP_altispeed_zone0_reset();

void APP_altispeed_zone3_redraw(int16_t airspeed);
APP_DrawPriority APP_altispeed_zone3_check(int16_t airspeed);
void APP_altispeed_zone3_reset();
void APP_altispeed_zone3_precalc(int16_t altispeedParams[]);

void APP_altispeed_zone5_redraw(int16_t airspeed);
APP_DrawPriority APP_altispeed_zone5_check(int16_t airspeed);
void APP_altispeed_zone5_reset();

/* Altitude zones */
void APP_altispeed_zone1_redraw(int16_t altitude);
APP_DrawPriority APP_altispeed_zone1_check(int16_t altitude);
void APP_altispeed_zone1_reset();

void APP_altispeed_zone4_redraw(int16_t altitude);
APP_DrawPriority APP_altispeed_zone4_check(int16_t altitude);
void APP_altispeed_zone4_reset();

void APP_altispeed_zone6a_redraw(int16_t altitude);
void APP_altispeed_zone6b_redraw(int16_t altitude);
APP_DrawPriority APP_altispeed_zone6a_check(int16_t altitude);
APP_DrawPriority APP_altispeed_zone6b_check(int16_t altitude);
void APP_altispeed_zone6a_reset();
void APP_altispeed_zone6b_reset();

#endif /* SRC_FLIGHT_INSTRUMENTS_ALTISPEED_DRAW_H_ */
