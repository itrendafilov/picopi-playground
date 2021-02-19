/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <grlib.h>
#include "displays/displays.h"

// Fill a triangle - Bresenham method
// Original from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void Graphics_fillTriangle(const Graphics_Context *context, int x1, int y1,int x2, int y2, int x3, int y3)
{
    int temp;
    // Sort by Y coordinate
    if (y1 > y2)
    {
        temp = y1;
        y1 = y2;
        y2 = temp;
        temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y3)
    {
        temp = y1;
        y1 = y3;
        y3 = temp;
        temp = x1;
        x1 = x3;
        x3 = temp;
    }
    if (y2 > y3)
    {
        temp = y2;
        y2 = y3;
        y3 = temp;
        temp = x2;
        x2 = x3;
        x3 = temp;
    }

    uint16_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int16_t i, signx1, signx2, dx1, dy1, dx2, dy2;
    uint16_t e1, e2;

    t1x = t2x = x1;
    y = y1;   // Starting points

    dx1 = x2 - x1;
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = y2 - y1;

    dx2 = x3 - x1;
    if (dx2 < 0)
    {
        dx2 = -dx2;
        signx2 = -1;
    }
    else
        signx2 = 1;
    dy2 = y3 - y1;

    if (dy1 > dx1)
    {   // swap values
        temp = dx1;
        dx1 = dy1;
        dy1 = temp;
        changed1 = true;
    }
    if (dy2 > dx2)
    {   // swap values
        temp = dx2;
        dx2 = dy2;
        dy2 = temp;
        changed2 = true;
    }

    e2 = dx2 << 1;
    // Flat top, just process the second half
    if (y1 == y2)
        goto next;
    e1 = dx1 << 1;

    for (i = 0; i < dx1;)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                    t1xp = signx1;   //t1x += signx1;
                else
                    goto next1;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
        next1:
        // process second line until y value is about to change
        while (1)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;   //t2x += signx2;
                else
                    goto next2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
        next2:
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        Graphics_drawLineH(context, minx, maxx, y); // Draw line from min to max points found on the y
        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2)
            break;

    }
    next:
    // Second half
    dx1 = x3 - x2;
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = y3 - y2;
    t1x = x2;

    if (dy1 > dx1)
    {   // swap values
        temp = dx1;
        dx1 = dy1;
        dy1 = temp;
        changed1 = true;
    }
    else
        changed1 = false;

    e1 = dx1 >> 1;

    for (i = 0; i <= dx1; i++)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    break;
                }   //t1x += signx1;
                else
                    goto next3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
        next3:
        // process second line until y value is about to change
        while (t2x != x3)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next4;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
        next4:

        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        Graphics_drawLineH(context, minx, maxx, y); // Draw line from min to max points found on the y

        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            return;
    }
}
