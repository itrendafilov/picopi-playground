/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef LIB_SPEEDMATH_SPEEDMATH_H_
#define LIB_SPEEDMATH_SPEEDMATH_H_

#include <stdint.h>

typedef unsigned long int bcd32_t;
extern bcd32_t hex2bcd(uint16_t x);

extern uint16_t mulBy10(uint16_t x);
extern uint16_t divBy10(uint16_t x);
extern uint16_t modBy10(uint16_t x);

extern uint16_t mulBy100(uint16_t x);
extern uint16_t divBy100(uint16_t x);
extern uint16_t modBy100(uint16_t x);

#endif /* LIB_SPEEDMATH_SPEEDMATH_H_ */
