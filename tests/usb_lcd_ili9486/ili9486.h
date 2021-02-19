/*
 * Copyright 2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef SRC_LIB_ILI9486_H_
#define SRC_LIB_ILI9486_H_

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

void ili9486_init();

void ili9486_write_command(uint8_t cmd);
void ili9486_write_data(uint16_t data);
void ili9486_repeat();

#endif /* SRC_LIB_ILI9486_H_ */
