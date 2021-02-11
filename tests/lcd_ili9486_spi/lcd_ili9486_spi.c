/**
 * Copyright (c) 2021 Ianislav Trendafilov
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

/* Example code to talk to a ili9486 LCD display driver.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor SPI) cannot be used at 5v.

   You will need to use a level shifter on the SPI lines if you want to run the
   board at 5v.

   GPIO 10 (pin 14) SCK/spi0_sclk -> SCL/SCK on ili9486 board
   GPIO 11 (pin 15) MOSI/spi0_tx -> SDA/SDI on ili9486 board
   GPIO 12 (pin 16) display reset signal -> RSX on ili9486 board
   GPIO 13 (pin 17) Data/Command -> DC on ili9486 board
   GPIO 14 (pin 19) Backlight -> BLK on ili9486 board
   3.3v (pin 3;6) -> VCC on ili9486 board
   GND (pin 38)  -> GND on ili9486 board
*/

#define PIN_SCK  10
#define PIN_MOSI 11
#define PIN_RSX  12
#define PIN_DCX  13
#define PIN_BLK  14

#define SPI_PORT spi1



static inline void rsx_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_RSX, 1);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void rsx_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_RSX, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void blk_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_BLK, 1);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void blk_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_BLK, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void dcx_data() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_DCX, 1);
    asm volatile("nop \n nop \n nop");
}

static inline void dcx_command() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_DCX, 0);
    asm volatile("nop \n nop \n nop");
}

static void ili9486_write_command(uint8_t cmd) {
    dcx_command();
    spi_write_blocking(SPI_PORT, &cmd, 1);
    dcx_data(); // keep in data mode as we expect to send more data then commands
}

static void ili9486_write_data(uint8_t data) {
    spi_write_blocking(SPI_PORT, &data, 1);
}


void ili9486_init() {
    // This example will use SPI1 at 25MHz.
    spi_init(SPI_PORT, 25 * 1000 * 1000);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_DCX);
    gpio_set_dir(PIN_DCX, GPIO_OUT);
    dcx_data(); // keep in data mode as we expect to send more data then commands

    gpio_init(PIN_BLK);
    gpio_set_dir(PIN_BLK, GPIO_OUT);
    blk_select();

    gpio_init(PIN_RSX);
    gpio_set_dir(PIN_RSX, GPIO_OUT);
    rsx_select();  // reset display sequence
    sleep_ms(5);
    rsx_deselect();
    sleep_us(20);
    rsx_select();
    sleep_ms(200);

    ili9486_write_command(0x01);
    sleep_ms(200); // ToDo: do we need 200 ms?

//    ili9486_write_command(0xC0);                       /* Power Control 1 */
//    ili9486_write_data(0x19);                          /* VREG1OUT=  5.1250 */
//    ili9486_write_data(0x1a);                          /* VREG2OUT= -5.1875 */
//
//    ili9486_write_command(0xC1);                       /* Power Control 2 */
//    ili9486_write_data(0x45);                          /* VGH = Vci1 x 5, VGL = - Vci1 x 3 */
//    ili9486_write_data(0x00);                          /* Vci1 = External VCI */
//
//    ili9486_write_command(0xC2);                       /* Power Control 3 */
//    ili9486_write_data(0x33);                          /* Frequency of the step-up circuit: 1/4/5 = 1H, 2/3 = 4H */
//
//    ili9486_write_command(0xC5);                       /* VCOM Control */
//    ili9486_write_data(0x00);                          /* Do not program NV Memory */
//    ili9486_write_data(0x28);                          /* VCOM = -1.375 */

//    ili9486_write_command(0xB1);                       /* Frame Rate Control */
//    ili9486_write_data(0xA0);
//    ili9486_write_data(0x11);

    ili9486_write_command(0xB4);                       /* Display Inversion Control */
    ili9486_write_data(0x02);                          /* 2-dot inversion           */

    ili9486_write_command(0xB6);                       /* Display Function Control */
    ili9486_write_data(0x00);                          /* Use system interface and internal clock */
    ili9486_write_data(0x42);                          /* G480->G1 direction. Scan every 5 frames */
    ili9486_write_data(0x3B);                          /* Drive 8*(val + 1)=480 lines */

    ili9486_write_command(0xB7);                       /* Entry Mode Set */
    ili9486_write_data(0x07);                          /* Normal display, disable low-voltage detection */

//    ili9486_write_command(0xE0);              //  Positive Gamma control
//    ili9486_write_data(0x1F);
//    ili9486_write_data(0x25);
//    ili9486_write_data(0x22);
//    ili9486_write_data(0x0B);
//    ili9486_write_data(0x06);
//    ili9486_write_data(0x0A);
//    ili9486_write_data(0x4E);
//    ili9486_write_data(0xC6);
//    ili9486_write_data(0x39);
//    ili9486_write_data(0x00);
//    ili9486_write_data(0x00);
//    ili9486_write_data(0x00);
//    ili9486_write_data(0x00);
//    ili9486_write_data(0x00);
//    ili9486_write_data(0x00);
//
//    ili9486_write_command(0xE1);              //  Negative Gamma control
//    ili9486_write_data(0x3F);
//    ili9486_write_data(0x3F);
//    ili9486_write_data(0x0F);
//    ili9486_write_data(0x1F);
//    ili9486_write_data(0x0F);
//    ili9486_write_data(0x46);
//    ili9486_write_data(0x49);
//    ili9486_write_data(0x31);
//    ili9486_write_data(0x05);
//    ili9486_write_data(0x09);
//    ili9486_write_data(0x03);
//    ili9486_write_data(0x1C);
//    ili9486_write_data(0x1A);
//    ili9486_write_data(0x00);
//    ili9486_write_data(0x00);

    ili9486_write_command(0x36);                       /* Memory Access Control */
    ili9486_write_data(0x48);                          /* Swap column order, set RGB color order */

    ili9486_write_command(0x3A);
    ili9486_write_data(0x66);   // 18 bit color (16 bit not supported in SPI)

    ili9486_write_command(0x11);                                                      /* Exit Sleep                   */
    sleep_ms(120); // ToDo: do we need 120ms

    ili9486_write_command(0x29);                                                      /* Display on                   */

}

int main() {
    stdio_init_all();

    ili9486_init();

    ili9486_write_command(0x2C);   /* Memory write               */

    for (;;) {
        for (int i = 0; i < 320*480; i++) {
            ili9486_write_data(0xFF);
            ili9486_write_data(0x00);
            ili9486_write_data(0x00);
        }
        sleep_ms(5000);

        for (int i = 0; i < 320*480; i++) {
            ili9486_write_data(0x00);
            ili9486_write_data(0xFF);
            ili9486_write_data(0x00);
        }
        sleep_ms(5000);

        for (int i = 0; i < 320*480; i++) {
            ili9486_write_data(0x00);
            ili9486_write_data(0x00);
            ili9486_write_data(0xFF);
        }
        sleep_ms(5000);
    }
    return 0;
}
