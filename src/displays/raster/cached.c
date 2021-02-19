/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include "displays/displays.h"

extern void ili9486_write_buffer(uint16_t* values, int16_t windowSize);
extern void ili9486_set_limits(int16_t sX, int16_t sY, int16_t mX, int16_t mY);

void Graphics_copyDisplayCustom(int x0, int y0, int offset,
                                const Graphics_Context *dest,
                                const Graphics_Context *src) {
    // We are in PORTRAIT mode, so X and Y are inverted
    int16_t x1 = x0 + src->display->width;
    int16_t y1 = y0 + src->display->heigth - 1;

    // Return without drawing anything if the entire rectangle is out of the clipping region.
    if((x0 > dest->clipRegion.xMax) ||
       (x1 < dest->clipRegion.xMin) ||
       (y0 > dest->clipRegion.yMax) ||
       (y1 < dest->clipRegion.yMin))
    {
        return;
    }


    uint16_t* displayData = (uint16_t*)src->display->displayData;
    bool clipped = false;
    //
    // Clip the X coordinates to the edges of the clipping region if necessary.
    //
    if(x0 < dest->clipRegion.xMin)
    {
        displayData += src->display->width * (dest->clipRegion.xMin - x0);
        x0 = dest->clipRegion.xMin;
        clipped = true;
    }
    if(x1 > dest->clipRegion.xMax)
    {
        x1 = dest->clipRegion.xMax;
        clipped = true;
    }

    //
    // Clip the Y coordinates to the edges of the clipping region if necessary.
    //
    if(y0 < dest->clipRegion.yMin)
    {
        y0 = dest->clipRegion.yMin;
        clipped = true;
    }
    if(y1 > dest->clipRegion.yMax)
    {
        displayData += y1 - dest->clipRegion.yMax;
        y1 = dest->clipRegion.yMax;
        clipped = true;
    }

    // move by offset positions
    displayData += (offset << 3) * src->display->heigth;

    if (!clipped) {
        ili9486_set_limits(MAPPED_X(x1, y1), MAPPED_Y(x0, y0), MAPPED_X(x0, y0), MAPPED_Y(x1, y1));
        ili9486_write_buffer(displayData, src->display->width * src->display->heigth);
    } else {
        ili9486_set_limits(MAPPED_X(x1, y1), MAPPED_Y(x0, y0), MAPPED_X(x0, y0), MAPPED_Y(x1, y1));
        y1 -= y0 - 1; // new height
        while (x1-- > x0) {
            ili9486_write_buffer(displayData, y1);
            displayData += src->display->heigth;
        }
    }
}
