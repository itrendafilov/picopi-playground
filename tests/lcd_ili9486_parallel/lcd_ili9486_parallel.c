/**
 * Copyright (c) 2021 Ianislav Trendafilov. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

/* Example code to talk to a ili9486 LCD display driver via a 16 bit parallel interface.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO cannot be used at 5v.

   GPIO0 (pin 1) Data bit 0 -> DB0 on ili9486 board
   GPIO1 (pin 2) Data bit 1 -> DB1 on ili9486 board
   ...
   GPIO15 (pin 20) Data bit 15 -> DB15 on ili9486 board
   
   GPIO22 (pin 29) Tearing Effect -> TE pin on ili9486 board

   GPIO26 (pin 31) Module reset -> RESX on ili9486 board
   GPIO27 (pin 32) Write enable -> WRX on ili9486 board
   GPIO28 (pin 34) Data/Command -> RS/DCX on ili9486 board

   3.3v (pin 36) -> VCC on ili9486 board
   GND (pin 38)  -> GND on ili9486 board

   GND (pin 38)  -> IM0 on ili9486 board
   3.3v (pin 36) -> IM1 on ili9486 board
   GND (pin 38)  -> IM2 on ili9486 board

   GPIO17 (pin 22) TE Validation
       -- Debug Only pin to validate the sync method externally
       -- Use oscilloscope and wire two channels to both pin 21 and 22
*/

#define PIN_VAL  17
#define PIN_TE   22
#define PIN_RESX 26
#define PIN_WRX  27
#define PIN_DCX  28

#define SPI_PORT spi1
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

void te_callback_sync(uint gpio, uint32_t events);  // Find an VSYNC sygnal and then reconfigure
void te_callback_run(uint gpio, uint32_t events);   // to the a free running VSYNC based on HSYNC count

volatile int hsync_row = 0;

/* 
 *  A timer callback that validates that the free-running counter on the Pico Pi
 *  is still in sync with the hardware VSYNC. If not it reconfigures back to the 
 *  te_callback_sync method that will recapture the VSYNC signal.
 *
 *  This is mostly required to restore when debugging the application so no more than 
 *  two frames are out of sync.
 */
void te_resync(void) {
    if (!gpio_get(PIN_TE)) {  // we are in an HSYNC
        // error condition - we were expecing to still be in VSYNC, but we are in HSYNC
        // so change the TE callback to synchronize again
        hsync_row = 0;
        gpio_set_irq_enabled_with_callback(PIN_TE, GPIO_IRQ_EDGE_RISE, true, &te_callback_sync);
    } // else in VSYNC so we are good

    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);
}

/*
 * Configures a timer callback in the future to validate the synchronization of the HSYNC
 */
static void validate_sync_in_us(uint32_t delay_us) {
    // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    // Set irq handler for alarm irq
    irq_set_exclusive_handler(ALARM_IRQ, te_resync);
    // Enable the alarm irq
    irq_set_enabled(ALARM_IRQ, true);
    // Enable interrupt in block and at processor

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper bits
    uint64_t target = timer_hw->timerawl + delay_us;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}

/*
 * Captures the TE signal VSYNC that is around 700 us long pulse, while
 * the HSYNC pulses are less than 2us long at a period of around 25us (refresh rate dependant)
 */
void te_callback_sync(uint gpio, uint32_t events) {
    // Algorithm: Sleep > 2 micro seconds, read GPIO pin value
    // if still high, then this is a VSYNC signal
    // else is a HSYNC signal
    sleep_us(5); // 1600ns is min, 5us to have reserve
    if (gpio_get(gpio)) {  // VSYNC
        // switch the callback to pico generated output
        hsync_row = 1;
        gpio_set_irq_enabled_with_callback(PIN_TE, GPIO_IRQ_EDGE_RISE, true, &te_callback_run);
    } // else HSYNC
}

/*
 * A free-running counter on which row is currently being updated.
 * Validate and resync when we expect to be in an HSYNC.
 */
void te_callback_run(uint gpio, uint32_t events) {
    if (!hsync_row) {
        gpio_put(PIN_VAL, 1);
        // Attempt resync (required when a debugger breakpoint is pausing the execution)
        validate_sync_in_us(5);
    } else {
        gpio_put(PIN_VAL, 0);
    }
    if (hsync_row == 479) {
        hsync_row = 0;
    } else {
        hsync_row++;
    }
}

static inline void rsx_select() {
    gpio_put(PIN_RESX, 0);  // Active low
}

static inline void rsx_deselect() {
    gpio_put(PIN_RESX, 1);
}

static inline void wrx_select() {
    gpio_put(PIN_WRX, 0);  // Active low
}

static inline void wrx_deselect() {
    gpio_put(PIN_WRX, 1);
}

static inline void dcx_data() {
    gpio_put(PIN_DCX, 1);
}

static inline void dcx_command() {
    gpio_put(PIN_DCX, 0);
}

static void ili9486_write_command(uint8_t cmd) {
    dcx_command();
    wrx_select();
    gpio_put_masked(0xFFFF, cmd);
    wrx_deselect(); // Transfer on rising edge
    dcx_data(); // keep in data mode as we expect to send more data then commands
}

static void ili9486_write_data(uint16_t data) {
    wrx_select();
    gpio_put_masked(0xFFFF, data);
    wrx_deselect(); // Transfer on rising edge
}

static void ili9486_repeat() {
    gpio_put(PIN_WRX, 0);  // Active low
    gpio_put(PIN_WRX, 1);
}

static inline void init_pin(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_put(pin, 0);
}


void ili9486_init() {
    // Swtich GPIO0-15 (DB0-15) to SIO mode
    for (int i = 0; i < 16; i++) {
        init_pin(i);
    }

    /* Setup Data/Command pin */
    gpio_init(PIN_DCX);
    gpio_set_dir(PIN_DCX, GPIO_OUT);
    dcx_data(); // keep in data mode as we expect to send more data then commands

    /* Setup Write Data pin */
    gpio_init(PIN_WRX);
    gpio_set_dir(PIN_WRX, GPIO_OUT);
    gpio_set_function(PIN_WRX, GPIO_FUNC_SIO);
    wrx_deselect();

    /* Setup reset display pin */
    gpio_init(PIN_RESX);
    gpio_set_dir(PIN_RESX, GPIO_OUT);
    rsx_deselect();  // reset display sequence
    sleep_ms(5);
    rsx_select();
    sleep_us(20);
    rsx_deselect();
    sleep_ms(200);

    ili9486_write_command(0x01);                         /* Soft Reset */
    sleep_ms(200); // ToDo: do we need 200 ms?

    ili9486_write_command(0xC0);                       /* Power Control 1 */
    ili9486_write_data(0x19);                          /* VREG1OUT=  5.1250 */
    ili9486_write_data(0x1a);                          /* VREG2OUT= -5.1875 */

    ili9486_write_command(0xC1);                       /* Power Control 2 */
    ili9486_write_data(0x45);                          /* VGH = Vci1 x 5, VGL = - Vci1 x 3 */
    ili9486_write_data(0x00);                          /* Vci1 = External VCI */

    ili9486_write_command(0xC2);                       /* Power Control 3 */
    ili9486_write_data(0x33);                          /* Frequency of the step-up circuit: 1/4/5 = 1H, 2/3 = 4H */

    ili9486_write_command(0xC5);                       /* VCOM Control */
    ili9486_write_data(0x00);                          /* Do not program NV Memory */
    ili9486_write_data(0x28);                          /* VCOM = -1.375 */

    ili9486_write_command(0xB1);                       /* Frame Rate Control */
    ili9486_write_data(0xA0);
    ili9486_write_data(0x11);

    ili9486_write_command(0xB4);                       /* Display Inversion Control */
    ili9486_write_data(0x02);                          /* 2-dot inversion           */

    ili9486_write_command(0xB6);                       /* Display Function Control */
    ili9486_write_data(0x00);                          /* Use system interface and internal clock */
    ili9486_write_data(0x42);                          /* G480->G1 direction. Scan every 5 frames */
    ili9486_write_data(0x3B);                          /* Drive 8*(val + 1)=480 lines */

    ili9486_write_command(0xB7);                       /* Entry Mode Set */
    ili9486_write_data(0x07);                          /* Normal display, disable low-voltage detection */

    ili9486_write_command(0xE0);              //  Positive Gamma control
    ili9486_write_data(0x1F);
    ili9486_write_data(0x25);
    ili9486_write_data(0x22);
    ili9486_write_data(0x0B);
    ili9486_write_data(0x06);
    ili9486_write_data(0x0A);
    ili9486_write_data(0x4E);
    ili9486_write_data(0xC6);
    ili9486_write_data(0x39);
    ili9486_write_data(0x00);
    ili9486_write_data(0x00);
    ili9486_write_data(0x00);
    ili9486_write_data(0x00);
    ili9486_write_data(0x00);
    ili9486_write_data(0x00);

    ili9486_write_command(0xE1);              //  Negative Gamma control
    ili9486_write_data(0x3F);
    ili9486_write_data(0x3F);
    ili9486_write_data(0x0F);
    ili9486_write_data(0x1F);
    ili9486_write_data(0x0F);
    ili9486_write_data(0x46);
    ili9486_write_data(0x49);
    ili9486_write_data(0x31);
    ili9486_write_data(0x05);
    ili9486_write_data(0x09);
    ili9486_write_data(0x03);
    ili9486_write_data(0x1C);
    ili9486_write_data(0x1A);
    ili9486_write_data(0x00);
    ili9486_write_data(0x00);

    ili9486_write_command(0x36);                       /* Memory Access Control */
    ili9486_write_data(0x48);                          /* Swap column order, set RGB color order */

    ili9486_write_command(0x3A);                       /* Interface Pixel Format */
    ili9486_write_data(0x55);                          /* 16-bit */

    ili9486_write_command(0x35);                       /* Tearing Effect Line ON */
    ili9486_write_data(0x01);                          /* Both V-Blanking and H-Blanking information */

    ili9486_write_command(0x11);                       /* Exit Sleep                   */
    sleep_ms(120); // ToDo: do we need 120ms

    ili9486_write_command(0x29);                       /* Display on                   */

}

int main() {
    gpio_init(PIN_VAL);
    gpio_set_dir(PIN_VAL, GPIO_OUT);
    gpio_set_function(PIN_VAL, GPIO_FUNC_SIO);
    gpio_put(PIN_VAL, 0);

    gpio_init(PIN_TE);
    gpio_set_dir(PIN_TE, GPIO_IN);
    gpio_set_function(PIN_TE, GPIO_FUNC_SIO);

    // Setup the method to read the Tearing Effect pin. It contains both the VSYNC and HSYNC signals
    gpio_set_irq_enabled_with_callback(PIN_TE, GPIO_IRQ_EDGE_RISE, true, &te_callback_sync);

    ili9486_init();

    ili9486_write_command(0x2C);   /* Memory write               */

    for (;;) {
        ili9486_write_data(0xF800); // Red once

        for (int i = 0; i < 320*480 - 1; i++) {
            ili9486_repeat(); // repeat last color
        }
        sleep_ms(5000);

        ili9486_write_data(0x07E0); // Green once
        for (int i = 0; i < 320*480 - 1; i++) {
            ili9486_repeat(); // repeat last color
        }
        sleep_ms(5000);

        ili9486_write_data(0x001F); // Blue once
        for (int i = 0; i < 320*480 - 1; i++) {
            ili9486_repeat(); // repeat last color
        }
        sleep_ms(5000);
    }

    return 0;
}
