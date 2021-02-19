/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include "numbers_ebrima.h"

#ifdef NUMBERS_EBRIMA_MEDIUM

static const unsigned char ebrima_pixels_medium0_2bpp[] =
{
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x6a, 0xaa, 0x40,
    0x01, 0xaa, 0xaa, 0x90,
    0x0e, 0xaa, 0xaa, 0xa4,
    0x06, 0xa4, 0x06, 0xa4,
    0x0a, 0xa0, 0x02, 0xab,
    0x3a, 0x90, 0x01, 0xab,
    0x1a, 0xb0, 0x01, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x01, 0xa9,
    0x3a, 0x90, 0x01, 0xab,
    0x3a, 0x90, 0x02, 0xa8,
    0x06, 0xa4, 0x06, 0xa4,
    0x06, 0xaa, 0xaa, 0xac,
    0x01, 0xaa, 0xaa, 0x90,
    0x00, 0x6a, 0xaa, 0x40,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x6a, 0xaa, 0x40,
    0x01, 0xaa, 0xaa, 0x90,
    0x0e, 0xaa, 0xaa, 0xa4,
    0x06, 0xa4, 0x06, 0xa4,
    0x0a, 0xa0, 0x02, 0xab,
    0x3a, 0x90, 0x01, 0xab,
    0x1a, 0xb0, 0x01, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x01, 0xa9,
    0x3a, 0x90, 0x01, 0xab,
    0x3a, 0x90, 0x02, 0xa8,
    0x06, 0xa4, 0x06, 0xa4,
    0x06, 0xaa, 0xaa, 0xac,
    0x01, 0xaa, 0xaa, 0x90,
    0x00, 0x6a, 0xaa, 0x40,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x6a, 0xaa, 0x40,
    0x01, 0xaa, 0xaa, 0x90,
    0x0e, 0xaa, 0xaa, 0xa4,
    0x06, 0xa4, 0x06, 0xa4,
    0x0a, 0xa0, 0x02, 0xab,
    0x3a, 0x90, 0x01, 0xab,
    0x1a, 0xb0, 0x01, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x01, 0xa9,
    0x3a, 0x90, 0x01, 0xab,
    0x3a, 0x90, 0x02, 0xa8,
    0x06, 0xa4, 0x06, 0xa4,
    0x06, 0xaa, 0xaa, 0xac,
    0x01, 0xaa, 0xaa, 0x90,
    0x00, 0x6a, 0xaa, 0x40,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x6a, 0xaa, 0x40,
    0x01, 0xaa, 0xaa, 0x90,
    0x0e, 0xaa, 0xaa, 0xa4,
    0x06, 0xa4, 0x06, 0xa4,
    0x0a, 0xa0, 0x02, 0xab,
    0x3a, 0x90, 0x01, 0xab,
    0x1a, 0xb0, 0x01, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x01, 0xa9,
    0x3a, 0x90, 0x01, 0xab,
    0x3a, 0x90, 0x02, 0xa8,
    0x06, 0xa4, 0x06, 0xa4,
    0x06, 0xaa, 0xaa, 0xac,
    0x01, 0xaa, 0xaa, 0x90,
    0x00, 0x6a, 0xaa, 0x40,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x36, 0xa7, 0x00,
    0x00, 0x6a, 0xaa, 0x40,
    0x01, 0xaa, 0xaa, 0x90,
    0x0e, 0xaa, 0xaa, 0xa4,
    0x06, 0xa4, 0x06, 0xa4,
    0x0a, 0xa0, 0x02, 0xab,
    0x3a, 0x90, 0x01, 0xab,
    0x1a, 0xb0, 0x01, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x03, 0xa9,
    0x1a, 0xb0, 0x01, 0xa9,
    0x3a, 0x90, 0x01, 0xab,
    0x3a, 0x90, 0x02, 0xa8,
    0x06, 0xa4, 0x06, 0xa4,
    0x06, 0xaa, 0xaa, 0xac,
    0x01, 0xaa, 0xaa, 0x90,
    0x00, 0x6a, 0xaa, 0x40,
    0x00, 0x36, 0xa7, 0x00
};

static const unsigned long ebrima_palette_medium0_2bpp[]=
{
    0x000000,   0xcbcbcb,   0xffffff,   0x717171
    //0x000000,     0x999999,   0xffffff,   0x242424
};

const Graphics_Image numbers_ebrima_medium0_2bpp =
{
    .bPP = IMAGE_FMT_2BPP_UNCOMP,
    .xSize = 16,
    .ySize = 108,
    .numColors = 4,
    .pPalette = (uint32_t*)ebrima_palette_medium0_2bpp,
    .pPixel = (uint8_t*)ebrima_pixels_medium0_2bpp,
};

#endif /* NUMBERS_EBRIMA_MEDIUM */
