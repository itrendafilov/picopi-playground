/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include <math.h>
#include "displays/displays.h"

// ILI9341 low-level methods
extern void ili9486_set_limits(int16_t sX, int16_t sY, int16_t mX, int16_t mY);
extern void ili9486_write_data(uint16_t value);
extern void ili9486_write_buffer(uint16_t* values, int16_t windowSize);

// Buffer with color copied 16 times
static uint16_t customLineColor[16];

void Graphics_drawLineHCustom(const Graphics_Context *context, int x1, int x2, int y) {

    int32_t  temp;

    //
    // Check the arguments.
    //
    assert(context);

    //
    // If the Y coordinate of this line is not in the clipping region, then
    // there is nothing to be done.
    //
    if((y < context->clipRegion.yMin) ||
           (y > context->clipRegion.yMax))
    {
        return;
    }

    //
    // Swap the X coordinates if the first is larger than the second.
    //
    if(x1 > x2)
    {
        temp = x1;
        x1 = x2;
        x2 = temp;
    }

    //
    // If the entire line is outside the clipping region, then there is nothing
    // to be done.
    //
    if((x1 > context->clipRegion.xMax) ||
       (x2 < context->clipRegion.xMin))
    {
        return;
    }

    //
    // Clip the starting coordinate to the left side of the clipping region if
    // required.
    //
    if(x1 < context->clipRegion.xMin)
    {
        x1 = context->clipRegion.xMin;
    }

    //
    // Clip the ending coordinate to the right side of the clipping region if
    // required.
    //
    if(x2 > context->clipRegion.xMax)
    {
        x2 = context->clipRegion.xMax;
    }


    // Draw the line
    ili9486_set_limits(MAPPED_X(x1, y), MAPPED_Y(x1, y), MAPPED_X(x1, y), LCD_Y_SIZE - 1);

    x2 -= x1 - 1;

    while (x2 > 16) {
        ili9486_write_buffer(customLineColor, 16);
        x2 -= 16;
    }
    if (x2 > 0) {
        ili9486_write_buffer(customLineColor, x2);
    }
}

void Graphics_drawLineVCustom(const Graphics_Context *context, int x, int y1, int y2) {
    int32_t  temp;

    //
    // Check the arguments.
    //
    assert(context);

    //
    // If the X coordinate of this line is not within the clipping region, then
    // there is nothing to be done.
    //
    if((x < context->clipRegion.xMin) ||
       (x > context->clipRegion.xMax))
    {
        return;
    }

    //
    // Swap the Y coordinates if the first is larger than the second.
    //
    if(y1 > y2)
    {
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    //
    // If the entire line is out of the clipping region, then there is nothing
    // to be done.
    //
    if((y1 > context->clipRegion.yMax) ||
       (y2 < context->clipRegion.yMin))
    {
        return;
    }

    //
    // Clip the starting coordinate to the top side of the clipping region if
    // required.
    //
    if(y1 < context->clipRegion.yMin)
    {
        y1 = context->clipRegion.yMin;
    }

    //
    // Clip the ending coordinate to the bottom side of the clipping region if
    // required.
    //
    if(y2 > context->clipRegion.yMax)
    {
        y2 = context->clipRegion.yMax;
    }


    // Draw the line
    ili9486_set_limits(MAPPED_X(x, y2), MAPPED_Y(x, y2), LCD_X_SIZE - 1, LCD_Y_SIZE - 1);

    y2 -= y1 - 1;

    while (y2 > 16) {
        ili9486_write_buffer(customLineColor, 16);
        y2 -= 16;
    }
    if (y2 > 0) {
        ili9486_write_buffer(customLineColor, y2);
    }
}


static void drawLineCustomByX(int x1, int y1, int dx, int dy, uint16_t color, bool incy)
{
    ili9486_set_limits(x1, y1, LCD_X_SIZE - 1, y1);
    x1 += dx;     // pre-calculate
    int len = dx; // Use as iterator

    dy <<= 1;  // Multiply dy by 2 to reduce calculations going forward
    int error = dx - dy; // substract threshold value (dx-2dy) as it is common
    dx <<= 1;  // Multiply dx by 2 to reduce calculations going forward

    // We draw left to right until we move past right point
    while (len-- >= 0)
    {
        ili9486_write_data(color);

        if (error > 0) // error + 2*dy > dx (pre-calculated)
        {
            if (incy) y1++;
            else y1--;
            ili9486_set_limits(x1 - len, y1, LCD_X_SIZE - 1, y1);

            error -= dx; // error - 2*dx (pre-calculated)
        }
        error += dy;     // error + 2*dy (pre-calculated)
    }
}

static void drawLineCustomByY(int x1, int y1, int dx, int dy, uint16_t color, bool incx)
{
    ili9486_set_limits(x1, y1, x1, LCD_Y_SIZE - 1);
    y1 += dy;     // pre-calculate
    int len = dy; // Use as iterator

    dx <<= 1;  // Multiply dx by 2 to reduce calculations going forward
    int error = dy - dx; // substract threshold value (2dx-dy) as it is common
    dy <<= 1;  // Multiply dy by 2 to reduce calculations going forward

    // We draw left to right until we move past right point
    while (len-- >= 0)
    {
        ili9486_write_data(color);

        if (error > 0) // error + 2*dx > dy (pre-calculated)
        {
            if (incx) x1++;
            else x1--;
            ili9486_set_limits(x1, y1 - len, x1, LCD_Y_SIZE - 1);

            error -= dy; // error - 2*dy (pre-calculated)
        }
        error += dx;     // error + 2*dx (pre-calculated)
    }
}

static void drawLineCustom(int x1, int y1, int x2, int y2, uint16_t color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dy < 0)
    {
        dy = y1 - y2; // recalculate is probably faster than multiply by -1
        if (dx < 0)
        {
            dx = x1 - x2; // recalculate is probably faster than multiply by -1
            if (dx > dy) drawLineCustomByX(x2, y2, dx, dy, color, true);
            else drawLineCustomByY(x2, y2, dx, dy, color, true);
        }
        else
        {
            if (dx > dy) drawLineCustomByX(x1, y1, dx, dy, color, false);
            else drawLineCustomByY(x2, y2, dx, dy, color, false);
        }
    }
    else
    {
        if (dx < 0)
        {
            dx = x1 - x2; // recalculate is probably faster than multiply by -1
            if (dx > dy) drawLineCustomByX(x2, y2, dx, dy, color, false);
            else drawLineCustomByY(x1, y1, dx, dy, color, false);
        }
        else
        {
            if (dx > dy) drawLineCustomByX(x1, y1, dx, dy, color, true);
            else drawLineCustomByY(x1, y1, dx, dy, color, true);
        }
    }

}

void Graphics_drawLineCustom(const Graphics_Context *context, int x1, int y1, int x2, int y2) {
    drawLineCustom(MAPPED_X(x1, y1), MAPPED_Y(x1, y1), MAPPED_X(x2, y2), MAPPED_Y(x2, y2), context->foreground);
}

void Graphics_drawLineAtAngle(const Graphics_Context *context, float angle, int centerX, int centerY, int d1, int d2)
{
    // ToDo: extract cos/sin or optimize by other means?
    int x1 = (int)round(d2 * cos(angle)) + centerX;
    int y1 = (int)round(d2 * sin(angle)) + centerY;
    int x2 = (int)round(d1 * cos(angle)) + centerX;
    int y2 = (int)round(d1 * sin(angle)) + centerY;

    // ToDo: Debug code, remove
    Graphics_setForegroundColor((Graphics_Context *)context, ClrBlack);
    Graphics_drawPixel(context, x1, y1);
    Graphics_drawPixel(context, x2, y2);

    Graphics_setForegroundColor((Graphics_Context *)context, ClrRed);
    Graphics_drawPixel(context, centerX, centerY);

    Graphics_drawLineCustom(context, x1, y1, x2, y2);
}


void Graphics_drawLineWidth(const Graphics_Context *context, int x0, int y0, int x1, int y1, int wd)
{
    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
    int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : sqrt((float)(dx*dx+dy*dy));
    int ew = wd >> 1;

    for (wd = (wd+1)/2; ; ) {                                   /* pixel loop */
       if (abs(abs(err-dx+dy)/ed-wd+1) < ew) {
           Graphics_drawPixel(context, x0, y0);
       }

       e2 = err; x2 = x0;
       if (2*e2 >= -dx) {                                           /* x step */
          for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx) {
             if ( abs(abs(e2)/ed-wd+1) < ew) Graphics_drawPixel(context, x0, y2);
             y2 += sy;
          }
          e2 = err; err -= dy; x0 += sx;
       }
       if (2*e2 <= dy) {                                            /* y step */
          for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy) {
              if ( abs(abs(e2)/ed-wd+1) < ew) Graphics_drawPixel(context, x2, y0);
              x2 += sx;
          }
          err += dx; y0 += sy;
       }
       if (x0 == x1 && y0 == y1) break;
    }
}
