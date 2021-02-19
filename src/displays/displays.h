/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef SRC_DISPLAYS_H_
#define SRC_DISPLAYS_H_

#include <grlib.h>
#include "config/board.h"

#ifdef Board_LCD_ILI9486
#include "drivers/ili9486_driver.h"
extern const Graphics_Display g_ili9486_Driver;
#endif /* Board_LCD_ILI9486 */


// Global graphics context
extern Graphics_Context g_sContext;

void Graphics_copyDisplayCustom(int x0, int y0, int offset,
                                const Graphics_Context *dest,
                                const Graphics_Context *src);

void Graphics_fillTriangle(const Graphics_Context *context,
                           int x1, int y1,int x2, int y2, int x3, int y3);
void Graphics_drawLineWidth(const Graphics_Context *context,
                            int x0, int y0, int x1, int y1, int wd);
void Graphics_drawLineCustom(const Graphics_Context *context,
                             int x1, int y1, int x2, int y2);

void Graphics_drawLineAtAngle(const Graphics_Context *context,
                              float angle, int centerX, int centerY, int d1, int d2);

void Graphics_drawDigitSmall(int x, int y, int digit);
void Graphics_drawDigitSmallRoll(int x, int y, int digit, int roll);

void Graphics_clearDigitMedium(int x, int y);
void Graphics_clearDigitMediumRollSmall(int x, int y);
void Graphics_drawDigitMedium(int x, int y, int digit);
void Graphics_drawDigitMediumRoll(int x, int y, int digit, int roll);
void Graphics_drawDigitMediumRollSmall(int x, int y, int digit, int roll);
void Graphics_drawDigitMediumRollZeros(int x, int y, int roll);

void Graphics_drawDigitMediumMemmap(int x, int y, int digit);
void Graphics_drawDigitMediumMemmapRoll(int x, int y, int digit, int roll);
void Graphics_drawDigitMediumMemmapRollSmall(int x, int y, int digit, int roll);

void Graphics_drawDigitBig(int x, int y, int digit);
void Graphics_drawDigitBigRoll(int x, int y, int digit, int roll);
void Graphics_drawDigitHuge(int x, int y, int digit);
void Graphics_drawDigitHugeRoll(int x, int y, int digit, int roll);

extern const Graphics_Font g_sFontEbrima10b;
extern const Graphics_Font g_sFontEbrima12b;
extern const Graphics_Font g_sFontEbrima16b;
extern const Graphics_Font g_sFontEbrima20b;
extern const Graphics_Font g_sFontEbrima24b;

extern const Graphics_Rectangle noScreenClipping;
#endif /* SRC_DISPLAYS_H_ */
