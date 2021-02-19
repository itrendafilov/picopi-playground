/*
 * Copyright(c) 2021 Ianislav Trendafilov. All rights reserved.
 */

#include "config/board.h"
#include <pico/stdlib.h>
#include <hardware/gpio.h>

void APP_led1_start(uint16_t period) {
}


void APP_led1_stop() {
}

void APP_led1_enable() {
    gpio_put(Board_LED0, Board_LED_ON);
}

void APP_led1_disable() {
    gpio_put(Board_LED0, Board_LED_OFF);
}

void APP_led1_toggle() {
    if (gpio_get(Board_LED0)) {
        APP_led1_disable();
    } else {
        APP_led1_enable;
    }
}
