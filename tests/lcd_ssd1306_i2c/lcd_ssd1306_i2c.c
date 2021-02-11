/**
 * Copyright (c) 2021 Ianislav Trendafilov
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

/* Example code to drive a 128x64 LCD panel via a I2C bridge chip (e.g. ssd1306)

   NOTE: The panel must be capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO 0 (pin 1)-> SDA on LCD bridge board
   GPIO 1 (pin 2)-> SCL on LCD bridge board
   3.3v (pin 36) -> VCC on LCD bridge board
   GND (pin 38)  -> GND on LCD bridge board
*/

#define I2C_PORT i2c0
// By default these LCD display drivers are on bus address 0x27
static int addr = 0x3c;

#define MAX_WIDTH       128
#define MAX_HEIGHT      64
#define MAX_PAGES       8  // MAX_HEIGHT / 8

uint8_t cmd_buf[] = { 0x00, 0x00 };
uint8_t dat_buf[] = { 0x40, 0x00 };
uint8_t addr_buf[] = { 0x21, 0x00, MAX_WIDTH - 1, 0x22, 0x00, MAX_PAGES - 1 };

/* Quick helper function for single byte transfers */
void ssd1306_write_cmd(uint8_t cmd) {
    cmd_buf[1] = cmd;
    i2c_write_blocking(I2C_PORT, addr, cmd_buf, 2, false);
}
void ssd1306_write_data(uint8_t data) {
    dat_buf[1] = data;
    i2c_write_blocking(I2C_PORT, addr, dat_buf, 2, false);
}

/* Frequently used functions */
void ssd1306_set_address(uint8_t x, uint8_t y) {
    addr_buf[1] = x;
    addr_buf[4] = y / 8;
    i2c_write_blocking(I2C_PORT, addr, addr_buf, 6, false);
}

void lcd_clear() {
    volatile int i,j;
    ssd1306_set_address(0,0);
    for (j = 0; j < MAX_PAGES; j++) {
        for (i = 0; i < MAX_WIDTH; i++) {
            ssd1306_write_data(0x00);
        }
    }
}

void lcd_init() {
    // Init LCD controller parameters
    ssd1306_write_cmd(0xAE); /* Entire Display OFF */
    ssd1306_write_cmd(0xD5); /* Set Display Clock Divide Ratio and Oscillator Frequency */
    ssd1306_write_cmd(0x80); /* Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended */
    ssd1306_write_cmd(0xA8); /* Set Multiplex Ratio */
    ssd1306_write_cmd(0x3F); /* 64 COM lines */
    ssd1306_write_cmd(0xD3); /* Set display offset */
    ssd1306_write_cmd(0x00); /* 0 offset */
    ssd1306_write_cmd(0x40); /* Set first line as the start line of the display */
    ssd1306_write_cmd(0x8D); /* Charge pump */
    ssd1306_write_cmd(0x14); /* Enable charge dump during display on */
    ssd1306_write_cmd(0x20); /* Set memory addressing mode */
    ssd1306_write_cmd(0x00); /* Horizontal addressing mode */
    ssd1306_write_cmd(0xA1); /* Set segment remap with column address 127 mapped to segment 0 */
    ssd1306_write_cmd(0xC8); /* Set com output scan direction, scan from com63 to com 0 */
    ssd1306_write_cmd(0xDA); /* Set com pins hardware configuration */
    ssd1306_write_cmd(0x12); /* Alternative com pin configuration, disable com left/right remap */
    ssd1306_write_cmd(0x81); /* Set contrast control */
    ssd1306_write_cmd(0x80); /* Set Contrast to 128 */
    ssd1306_write_cmd(0xD9); /* Set pre-charge period */
    ssd1306_write_cmd(0xF1); /* Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK */
    ssd1306_write_cmd(0xDB); /* Set Vcomh deselect level */
    ssd1306_write_cmd(0x20); /* Vcomh deselect level ~ 0.77 Vcc */
    ssd1306_write_cmd(0xA4); /* Entire display ON, resume to RAM content display */
    ssd1306_write_cmd(0xA6); /* Set Display in Normal Mode, 1 = ON, 0 = OFF */
    ssd1306_write_cmd(0x2E); /* Deactivate scroll */
    ssd1306_write_cmd(0xAF); /* Display on in normal mode */
    lcd_clear();
}

int main() {
    // This example will use I2C0 on GPIO0 (SDA) and GPIO1 (SCL)
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);
    // Make the I2C pins available to picotool
    bi_decl( bi_2pins_with_func(0, 1, GPIO_FUNC_I2C));

    lcd_init();

    static uint8_t message[] = {
        0, 4, 0, 4, 0, 4, 0, 4, 0, 0, 0,        // H
        0, 4, 0, 0, 0,                          // e
        0, 4, 0, 4, 4, 4, 0, 4, 0, 4, 0, 0, 0,  // l
        0, 4, 0, 4, 4, 4, 0, 4, 0, 4, 0, 0, 0,  // l
        0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4      // o
     };

    ssd1306_set_address(0,0);

    for (int i = 0; i < sizeof(message); i++) {
        ssd1306_write_data(message[i]);
    }

    return 0;
}
