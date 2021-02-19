/**
 * Copyright (c) 2021 Ianislav Trendafilov. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "bsp/board.h"
#include "tusb.h"
#include "ili9486.h"

uint colors[] = {0xF800, 0x07E0, 0x001F};
volatile uint current = 0;
void next_screen_color() {
    multicore_fifo_push_blocking(colors[current++]);
    if (current == 3) current = 0;
}

void core1_entry() {
    for (;;) {
        // Wait for a message
        uint32_t color = multicore_fifo_pop_blocking();
        ili9486_write_data(color); // Red once
        for (int i = 0; i < 320*480 - 1; i++) {
            ili9486_repeat(); // repeat last color
        }
    }
}

int main() {
    board_init();
    ili9486_init();
    ili9486_write_command(0x2C);   /* Memory write               */

    multicore_launch_core1(core1_entry);
    next_screen_color();
    
    tusb_init();

    for (;;) {
        tud_task(); // tinyusb device task
    }

    return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    // This example doesn't use multiple report and report ID
    (void) report_id;
    (void) report_type;

    next_screen_color();

    // echo back anything we received from host
    tud_hid_report(0, buffer, bufsize);
}
