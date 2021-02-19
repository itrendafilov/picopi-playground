/*
 * Copyright (c) 2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef __ILI9486_Driver_H__
#define __ILI9486_Driver_H__

#include <grlib.h>
#define LCD_X_SIZE      320
#define LCD_Y_SIZE      480
#define BPP             16

// LCD Screen Orientation Here
#define LANDSCAPE
//#define LANDSCAPE_FLIP
//#define PORTRAIT
//#define PORTRAIT_FLIP

#if !defined(PORTRAIT) && !defined(PORTRAIT_FLIP) && \
    !defined(LANDSCAPE) && !defined(LANDSCAPE_FLIP)
#define LANDSCAPE
#endif

//*****************************************************************************
//
// Various definitions controlling coordinate space mapping and drawing
// direction in the four supported orientations.
//
//*****************************************************************************
#ifdef PORTRAIT
#define MAPPED_X(x, y) (LCD_X_SIZE - (y) - 1)
#define MAPPED_Y(x, y) (x)
#endif
#ifdef LANDSCAPE
#define MAPPED_X(x, y) (x)
#define MAPPED_Y(x, y) (y)
#endif
#ifdef PORTRAIT_FLIP
#define MAPPED_X(x, y)  (y)
#define MAPPED_Y(x, y)  (LCD_Y_SIZE - (x) - 1)
#endif
#ifdef LANDSCAPE_FLIP
#define MAPPED_X(x, y)  (LCD_X_SIZE - (x) - 1)
#define MAPPED_Y(x, y)  (LCD_Y_SIZE - (y) - 1)
#endif

//*****************************************************************************
//
// Macros for the Display Driver
//
//*****************************************************************************
//#define DPYCOLORTRANSLATE(c)    ((((((c) & 0x00ff0000) >> 16) * 19661) + \
//                                  ((((c) & 0x0000ff00) >> 8) * 38666) +  \
//                                  (((c) & 0x000000ff) * 7209)) /         \
//                                 (65536 * 128))
#define RGBA888_TO_RGB565(c) \
    (((((((c) & 0x00ff0000) >> 16) * 249 + 1014) >> 11) << 11) | \
     ((((((c) & 0x0000ff00) >> 8) * 253 + 505) >> 10) << 5 )|  \
       ((((c) & 0x000000ff) * 249 + 1014) >> 11))

#define DPYCOLORTRANSLATE(c) RGBA888_TO_RGB565(c)

//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************
extern void ili9486_DriverInit(void);
extern const Graphics_Display g_ili9486_Driver;

#endif // __ILI9486_Driver_H__
