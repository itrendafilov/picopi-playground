/*
 * Copyright (c) 2021 Ianislav Trendafilov. All rights reserved.
 */

#include "ili9486_driver.h"
#include <grlib.h>
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

static inline void ili9486_repeat() {
    gpio_put(PIN_WRX, 0);  // Active low
    gpio_put(PIN_WRX, 1);
}

static void ili9486_init() {
    gpio_init(PIN_VAL);
    gpio_set_dir(PIN_VAL, GPIO_OUT);
    gpio_set_function(PIN_VAL, GPIO_FUNC_SIO);
    gpio_put(PIN_VAL, 0);

    gpio_init(PIN_TE);
    gpio_set_dir(PIN_TE, GPIO_IN);
    gpio_set_function(PIN_TE, GPIO_FUNC_SIO);

    // Setup the method to read the Tearing Effect pin. It contains both the VSYNC and HSYNC signals
    gpio_set_irq_enabled_with_callback(PIN_TE, GPIO_IRQ_EDGE_RISE, true, &te_callback_sync);

    // Swtich GPIO0-15 (DB0-15) to SIO mode
    for (int pin = 0; pin < 16; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_set_function(pin, GPIO_FUNC_SIO);
        gpio_put(pin, 0);
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

void ili9486_set_limits(int16_t sX, int16_t sY, int16_t mX, int16_t mY)
{
    ili9486_write_command(0x2A);   /* Column Command address     */
    ili9486_write_data(sX >> 8);   /*    start column [15:8]     */
    ili9486_write_data(sX & 0xFF);  /*    start column [7:0]      */
    ili9486_write_data(mX >> 8);   /*    max column [15:8]       */
    ili9486_write_data(mX & 0xFF);  /*    max column [7:0]        */
    ili9486_write_command(0x2B);   /* Page Command address       */
    ili9486_write_data(sY >> 8);   /*    start page [15:8]       */
    ili9486_write_data(sY & 0xFF);  /*    start page [7:0]        */
    ili9486_write_data(mY >> 8);   /*    max page [15:8]         */
    ili9486_write_data(mY & 0xFF);  /*    max page [7:0]          */
    ili9486_write_command(0x2C);   /* Memory write               */
}


// Sets the pixel address of the LCD driver
inline void ili9486_set_position(int16_t lX, int16_t lY)
{
    ili9486_set_limits(lX, lY, LCD_X_SIZE - 1, LCD_Y_SIZE - 1);
}

void ili9486_write_repeat(uint16_t ulValue, uint16_t times) {
    if (times--) {
        ili9486_write_data(ulValue); // write once

        for (int i = 0; i < times; i++) {
            ili9486_repeat(); // repeat last color
        }
    }
}

//*****************************************************************************
//
// All the following functions (below) for the LCD driver are required by grlib
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
// TemplateDisplayFix
static void
ili9486_DriverPixelDraw(void *pvDisplayData,
                         int16_t lX,
                         int16_t lY,
                         uint16_t ulValue)
{
    ili9486_set_position(MAPPED_X(lX, lY), MAPPED_Y(lX, lY));
    ili9486_write_data(ulValue); // Pixel value
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
ili9486_DriverPixelDrawMultiple(void *pvDisplayData,
                                 int16_t lX,
                                 int16_t lY,
                                 int16_t lX0,
                                 int16_t lCount,
                                 int16_t lBPP,
                                 const uint8_t *pucData,
                                 const uint32_t *pucPalette)
{
    uint16_t ulByte;

    // draw one pixel per line with up to lCount
    ili9486_set_limits(MAPPED_X(lX, lY), MAPPED_Y(lX, lY), MAPPED_X(lX, lY), LCD_Y_SIZE - 1); // MAPPED_Y(lX, lY) + lCount);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
    // The pixel data is in 1 bit per pixel format
    case 1:
    {
        // Loop while there are more pixels to draw
        while(lCount > 0)
        {
            // Get the next byte of image data
            lX = *pucData++;

            // Loop through the pixels in this byte of image data
            for(; (lX0 < 8) && lCount; lX0++, lCount--)
            {
                ulByte = (lX >> (7 - lX0)) & 1; // get pixel value
                ulByte = (*(uint16_t *)(pucPalette + ulByte));
                // Draw this pixel in the appropriate color
                ili9486_write_data(ulByte);
            }

            // Start at the beginning of the next byte of image data
            lX0 = 0;
        }
        // The image data has been drawn

        break;
    }

    // The pixel data is in 2 bit per pixel format
    case 2:
    {
        // Loop while there are more pixels to draw
        while(lCount > 0)
        {
            // Get the next byte of image data
            lX = *pucData++;

            // Loop through the pixels in this byte of image data
            for(; (lX0 < 4) && lCount; lX0++, lCount--)
            {
                ulByte = (lX >>(6 - (lX0 << 1))) & 3; // get pixel value
                ulByte = (*(uint16_t *)(pucPalette + ulByte));
                // Draw this pixel in the appropriate color
                ili9486_write_data(ulByte);
            }

            // Start at the beginning of the next byte of image data
            lX0 = 0;
        }
        // The image data has been drawn

        break;
    }
    // The pixel data is in 4 bit per pixel format
    case 4:
    {
        // Loop while there are more pixels to draw.  "Duff's device" is
        // used to jump into the middle of the loop if the first nibble of
        // the pixel data should not be used.  Duff's device makes use of
        // the fact that a case statement is legal anywhere within a
        // sub-block of a switch statement.  See
        // http://en.wikipedia.org/wiki/Duff's_device for detailed
        // information about Duff's device.
        switch(lX0 & 1)
        {
        case 0:
            // Cache the pallete colors in registers
            while(lCount)
            {
                // Get the upper nibble of the next byte of pixel data
                // and extract the corresponding entry from the palette
                ulByte = (*pucData >> 4);
                ulByte = (*(uint16_t *)(pucPalette + ulByte));
                // Write to LCD screen
                ili9486_write_data(ulByte); // Pixel value

                // Decrement the count of pixels to draw
                lCount--;

                // See if there is another pixel to draw
                if(lCount)
                {
                case 1:
                    // Get the lower nibble of the next byte of pixel
                    // data and extract the corresponding entry from
                    // the palette
                    ulByte = (*pucData++ & 15);
                    ulByte = (*(uint16_t *)(pucPalette + ulByte));
                    // Write to LCD screen
                    ili9486_write_data(ulByte); // Pixel value

                    // Decrement the count of pixels to draw
                    lCount--;
                }
            }
        }
        // The image data has been drawn.

        break;
    }

    // The pixel data is in 8 bit per pixel format
    case 8:
    {
        // Loop while there are more pixels to draw
        while(lCount--)
        {
            // Get the next byte of pixel data and extract the
            // corresponding entry from the palette
            ulByte = *pucData++;
            ulByte = (*(uint16_t *)(pucPalette + ulByte));
            // Write to LCD screen
            ili9486_write_data(ulByte); // Pixel value
        }
        // The image data has been drawn
        break;
    }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
ili9486_DriverLineDrawH(void *pvDisplayData,
                         int16_t lX1,
                         int16_t lX2,
                         int16_t lY,
                         uint16_t ulValue)
{
    // Limit to one pixel per row
    ili9486_set_limits(MAPPED_X(lX1, lY), MAPPED_Y(lX1, lY), MAPPED_X(lX1, lY), LCD_Y_SIZE - 1);
    ili9486_write_repeat(ulValue, lX2 - lX1 + 1);
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
ili9486_DriverLineDrawV(void *pvDisplayData,
                         int16_t lX,
                         int16_t lY1,
                         int16_t lY2,
                         uint16_t ulValue)
{
    ili9486_set_position(MAPPED_X(lX, lY2), MAPPED_Y(lX, lY2));
    ili9486_write_repeat(ulValue, lY2 - lY1 + 1);
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void
ili9486_DriverRectFill(void *pvDisplayData,
                        const Graphics_Rectangle *pRect,
                        uint16_t ulValue)
{
    int16_t x0 = pRect->sXMin;
    int16_t x1 = pRect->sXMax;
    int16_t y0 = pRect->sYMin;
    int16_t y1 = pRect->sYMax;

    // Create an memory addressing box, so we fill only inside the box
    ili9486_set_limits(MAPPED_X(x1, y1), MAPPED_Y(x0, y0), MAPPED_X(x0, y0), MAPPED_Y(x1, y1));

    y1 -= --y0; // y1 - y0 + 1 pre-calculated
    x1 -= --x0; // x1 - x0 + 1 pre-calculated

    ili9486_write_repeat(ulValue, x1 * y1);
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static uint32_t
ili9486_DriverColorTranslate(void *pvDisplayData,
                              uint32_t ulValue)
{
    /* The DPYCOLORTRANSLATE macro should be defined in TemplateDriver.h */

    //
    // Translate from a 24-bit RGB color to a color accepted by the LCD.
    //
    return(DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.
//!
//! \return None.
//
//*****************************************************************************
static void
ili9486_DriverFlush(void *pvDisplayData)
{
    // Flush Buffer here. This function is not needed if a buffer is not used,
    // or if the buffer is always updated with the screen writes.

    // Send all columns in all pages starting from (0,0)
    ili9486_set_position(0, 0);
}

//*****************************************************************************
//
//! Send command to clear screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This function does a clear screen and the Display Buffer contents
//! are initialized to the current background color.
//!
//! \return None.
//
//*****************************************************************************
static void
ili9486_DriverClearScreen(void *pvDisplayData,
                          uint16_t ulValue)
{
    // This fills the entire display to clear it
    ili9486_set_position(0, 0);

    ili9486_write_data(ulValue); // write once

    for (int i = 0; i < LCD_X_SIZE*LCD_Y_SIZE - 1; i++) {
        ili9486_repeat(); // repeat last color
    }
}

//*****************************************************************************
//
//! The display structure that describes the driver for the blank template.
//
//*****************************************************************************
const Graphics_Display g_ili9486_Driver =
{
    sizeof(tDisplay),
    NULL, // No memory buffer
#if defined(PORTRAIT) || defined(PORTRAIT_FLIP)
    LCD_Y_SIZE,
    LCD_X_SIZE,
#else
    LCD_X_SIZE,
    LCD_Y_SIZE,
#endif
    ili9486_DriverPixelDraw,
    ili9486_DriverPixelDrawMultiple,
    ili9486_DriverLineDrawH,
    ili9486_DriverLineDrawV,
    ili9486_DriverRectFill,
    ili9486_DriverColorTranslate,
    ili9486_DriverFlush,
    ili9486_DriverClearScreen
};

// Initializes the display driver.
// This function initializes the LCD controller
void ili9486_DriverInit(void) {
    ili9486_init();
    
    ili9486_DriverClearScreen(0, 0x0000); // this is a hack around library boundary
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
//*****************************************************************************
