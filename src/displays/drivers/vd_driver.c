/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

/* BIOS, TI-RTOS and XDCtools Header files */
#include <grlib.h>
#include "vd_driver.h"

//*****************************************************************************
//
// Virtual display properties
//
//*****************************************************************************
#define BPP             16
#define VD_X_SIZE       11
#define VD_Y_SIZE       8

//#define VDMODE_RW


#define RGBA888_TO_RGB565(c) \
    (((((((c) & 0x00ff0000) >> 16) * 249 + 1014) >> 11) << 11) | \
     ((((((c) & 0x0000ff00) >> 8) * 253 + 505) >> 10) << 5 )|  \
       ((((c) & 0x000000ff) * 249 + 1014) >> 11))

#define DPYCOLORTRANSLATE(c) RGBA888_TO_RGB565(c)


//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************

#ifdef VDMODE_RW
static uint16_t memoryBuffer[VD_X_SIZE * VD_Y_SIZE];
#else
extern const unsigned int pixel_numbers_memmap[];
#endif

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
vd_DriverPixelDraw(void *pvDisplayData,
                         int16_t lX,
                         int16_t lY,
                         uint16_t ulValue)
{
#ifdef VDMODE_RW
    int16_t* ptr = (int16_t*)pvDisplayData;
    *(ptr + lY + lX * VD_X_SIZE) = ulValue;
#endif
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
vd_DriverPixelDrawMultiple(void *pvDisplayData,
                                 int16_t lX,
                                 int16_t lY,
                                 int16_t lX0,
                                 int16_t lCount,
                                 int16_t lBPP,
                                 const uint8_t *pucData,
                                 const uint32_t *pucPalette)
{

#ifdef VDMODE_RW
    uint16_t ulByte;

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
            ulByte = *pucData++;

            // Loop through the pixels in this byte of image data
            for(; (lX0 < 8) && lCount; lX0++, lCount--)
            {
                // Draw this pixel in the appropriate color
                vd_DriverPixelDraw(pvDisplayData, lX++, lY,
                                         ((uint16_t *)pucPalette)[(ulByte >>
                                                                   (7 -
                                                                    lX0)) & 1]);
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
            ulByte = *pucData++;

            // Loop through the pixels in this byte of image data
            for(; (lX0 < 4) && lCount; lX0++, lCount--)
            {
                // Draw this pixel in the appropriate color
                vd_DriverPixelDraw(pvDisplayData, lX++, lY,
                                         ((uint16_t *)pucPalette)[(ulByte >>
                                                                   (6 -
                                                                    (lX0 <<
                    1))) & 3]);
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

            while(lCount)
            {
                // Get the upper nibble of the next byte of pixel data
                // and extract the corresponding entry from the palette
                ulByte = (*pucData >> 4);
                ulByte = (*(uint16_t *)(pucPalette + ulByte));
                // Write to LCD screen
                vd_DriverPixelDraw(pvDisplayData, lX++, lY, ulByte);

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
                    vd_DriverPixelDraw(pvDisplayData, lX++, lY, ulByte);

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
            vd_DriverPixelDraw(pvDisplayData, lX++, lY, ulByte);
        }
        // The image data has been drawn
        break;
    }
    }
#endif
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
vd_DriverLineDrawH(void *pvDisplayData,
                         int16_t lX1,
                         int16_t lX2,
                         int16_t lY,
                         uint16_t ulValue)
{
    do
    {
        vd_DriverPixelDraw(pvDisplayData, lX1, lY, ulValue);
    }
    while(lX1++ < lX2);
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
vd_DriverLineDrawV(void *pvDisplayData,
                         int16_t lX,
                         int16_t lY1,
                         int16_t lY2,
                         uint16_t ulValue)
{
    do
    {
        vd_DriverPixelDraw(pvDisplayData, lX, lY1, ulValue);
    }
    while(lY1++ < lY2);
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
vd_DriverRectFill(void *pvDisplayData,
                        const Graphics_Rectangle *pRect,
                        uint16_t ulValue)
{
    int16_t x0 = pRect->sXMin;
    int16_t x1 = pRect->sXMax;
    int16_t y0 = pRect->sYMin;
    int16_t y1 = pRect->sYMax;

    while(y0++ <= y1)
    {
        vd_DriverLineDrawH(pvDisplayData, x0, x1, y0, ulValue);
    }
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
vd_DriverColorTranslate(void *pvDisplayData,
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
vd_DriverFlush(void *pvDisplayData)
{
    // Flush Buffer here. This function is not needed if a buffer is not used,
    // or if the buffer is always updated with the screen writes.
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
vd_DriverClearScreen(void *pvDisplayData, uint16_t ulValue)
{
    uint16_t *ptr = pvDisplayData;
    for (int i = VD_X_SIZE * VD_Y_SIZE; i > 0; i--,ptr++) {
        *ptr = ulValue;
    }
}

//*****************************************************************************
//
//! The display structure that describes the driver for the blank template.
//
//*****************************************************************************
const Graphics_Display g_vdDisplay =
{
    sizeof(Graphics_Display),
#ifdef VDMODE_RW
    (void*)memoryBuffer,
#else
    (void*)pixel_numbers_memmap,
#endif
    VD_Y_SIZE, // we are in PORTRAIT mode thus reverse X and Y
    VD_X_SIZE,
    vd_DriverPixelDraw,
    vd_DriverPixelDrawMultiple,
    vd_DriverLineDrawH,
    vd_DriverLineDrawV,
    vd_DriverRectFill,
    vd_DriverColorTranslate,
    vd_DriverFlush,
    vd_DriverClearScreen
};

Graphics_Context g_vdContext;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
//*****************************************************************************
