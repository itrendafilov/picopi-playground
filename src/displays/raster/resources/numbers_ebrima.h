/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef NUMBERS_EBRIMA_H_
#define NUMBERS_EBRIMA_H_

#include <grlib.h>
#include <math.h>
#include "displays/displays.h"

#define NUMBERS_EBRIMA_SMALL
#define NUMBERS_EBRIMA_MEDIUM
#define NUMBERS_EBRIMA_MEDIUM_MEMMAP
//#define NUMBERS_EBRIMA_BIG
//#define NUMBERS_EBRIMA_HUGE

/* Helper variables */
#define CALC_EBRIMA_SMALL_WIDTH                12
#define CALC_EBRIMA_MEDIUM_WIDTH               16
#define CALC_EBRIMA_BIG_WIDTH                  20
#define CALC_EBRIMA_HUGE_WIDTH                 24

#define CALC_EBRIMA_SMALL_HEIGHT               15
#define CALC_EBRIMA_MEDIUM_HEIGHT              20
#define CALC_EBRIMA_BIG_HEIGHT                 26
#define CALC_EBRIMA_HUGE_HEIGHT                34

#define CALC_EBRIMA_SMALL_HEIGHT_SPACED        16  // 16 = 15 rows + 1 rows space
#define CALC_EBRIMA_MEDIUM_HEIGHT_SPACED       22  // 22 = 20 rows + 2 rows space
#define CALC_EBRIMA_BIG_HEIGHT_SPACED          29  // 29 = 26 rows + 3 rows space
#define CALC_EBRIMA_HUGE_HEIGHT_SPACED         38  // 38 = 34 rows + 4 rows space

#define CALC_EBRIMA_SMALL_BYTES_PER_ROW        3   // = 12 pixels width / (8 bits / 2 BPP)
#define CALC_EBRIMA_MEDIUM_BYTES_PER_ROW       4   // = 16 pixels width / (8 bits / 2 BPP)
#define CALC_EBRIMA_BIG_BYTES_PER_ROW          5   // = 20 pixels width / (8 bits / 2 BPP)
#define CALC_EBRIMA_HUGE_BYTES_PER_ROW         6   // = 24 pixels width / (8 bits / 2 BPP)

#define CALC_EBRIMA_DIGIT_ROW(digit, h, len)   (h * len * (10 - digit + 1)) // digit+1 as we start from nine for roll
#define CALC_EBRIMA_SMALL_DIGIT_ROW(digit)     CALC_EBRIMA_DIGIT_ROW(digit, CALC_EBRIMA_SMALL_HEIGHT_SPACED, CALC_EBRIMA_SMALL_BYTES_PER_ROW)
#define CALC_EBRIMA_MEDIUM_DIGIT_ROW(digit)    CALC_EBRIMA_DIGIT_ROW(digit, CALC_EBRIMA_MEDIUM_HEIGHT_SPACED, CALC_EBRIMA_MEDIUM_BYTES_PER_ROW)
#define CALC_EBRIMA_MEDIUM_ZEROS_DIGIT_ROW     CALC_EBRIMA_DIGIT_ROW(8, CALC_EBRIMA_MEDIUM_HEIGHT_SPACED, CALC_EBRIMA_MEDIUM_BYTES_PER_ROW)
#define CALC_EBRIMA_BIG_DIGIT_ROW(digit)       CALC_EBRIMA_DIGIT_ROW(digit, CALC_EBRIMA_BIG_HEIGHT_SPACED, CALC_EBRIMA_BIG_BYTES_PER_ROW)
#define CALC_EBRIMA_HUGE_DIGIT_ROW(digit)      CALC_EBRIMA_DIGIT_ROW(digit, CALC_EBRIMA_HUGE_HEIGHT_SPACED, CALC_EBRIMA_HUGE_BYTES_PER_ROW)

#define CALC_EBRIMA_MEDIUM_DIGIT_MM(digit)     (CALC_EBRIMA_MEDIUM_HEIGHT_SPACED * (digit + 1) - 1) // We always try to skip the nine used for roll
#define CALC_EBRIMA_MEDIUM_DIGIT_MM_ROW_LEN    284

#define CALC_EBRIMA_DIGIT_CENTER(h, len)       ((h >> 1) * len)
#define CALC_EBRIMA_SMALL_DIGIT_CENTER         CALC_EBRIMA_DIGIT_CENTER(CALC_EBRIMA_SMALL_HEIGHT, CALC_EBRIMA_SMALL_BYTES_PER_ROW)
#define CALC_EBRIMA_MEDIUM_DIGIT_CENTER        CALC_EBRIMA_DIGIT_CENTER(CALC_EBRIMA_MEDIUM_HEIGHT, CALC_EBRIMA_MEDIUM_BYTES_PER_ROW)
#define CALC_EBRIMA_BIG_DIGIT_CENTER           CALC_EBRIMA_DIGIT_CENTER(CALC_EBRIMA_BIG_HEIGHT, CALC_EBRIMA_BIG_BYTES_PER_ROW)
#define CALC_EBRIMA_HUGE_DIGIT_CENTER          CALC_EBRIMA_DIGIT_CENTER(CALC_EBRIMA_HUGE_HEIGHT, CALC_EBRIMA_HUGE_BYTES_PER_ROW)

#define CALC_EBRIMA_SMALL_ROLL_Y               12
#define CALC_EBRIMA_MEDIUM_ROLL_Y              16
#define CALC_EBRIMA_MEDIUM_ROLL_SMALL_Y        8
#define CALC_EBRIMA_BIG_ROLL_Y                 20
#define CALC_EBRIMA_HUGE_ROLL_Y                24

#define CALC_EBRIMA_DIGIT_ROLL_PIX(roll, h)    divBy10(h * roll)
#define CALC_EBRIMA_DIGIT_ROLL(roll, h, len)   (CALC_EBRIMA_DIGIT_ROLL_PIX(roll, h) * len)
#define CALC_EBRIMA_SMALL_DIGIT_ROLL(roll)     CALC_EBRIMA_DIGIT_ROLL(roll, CALC_EBRIMA_SMALL_HEIGHT_SPACED, CALC_EBRIMA_SMALL_BYTES_PER_ROW)
#define CALC_EBRIMA_MEDIUM_DIGIT_ROLL(roll)    CALC_EBRIMA_DIGIT_ROLL(roll, CALC_EBRIMA_MEDIUM_HEIGHT_SPACED, CALC_EBRIMA_MEDIUM_BYTES_PER_ROW)
#define CALC_EBRIMA_BIG_DIGIT_ROLL(roll)       CALC_EBRIMA_DIGIT_ROLL(roll, CALC_EBRIMA_BIG_HEIGHT_SPACED, CALC_EBRIMA_BIG_BYTES_PER_ROW)
#define CALC_EBRIMA_HUGE_DIGIT_ROLL(roll)      CALC_EBRIMA_DIGIT_ROLL(roll, CALC_EBRIMA_HUGE_HEIGHT_SPACED, CALC_EBRIMA_HUGE_BYTES_PER_ROW)

#endif /* NUMBERS_EBRIMA_H_ */
