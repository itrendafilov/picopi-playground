/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include "numbers_ebrima.h"

#ifdef NUMBERS_EBRIMA_SMALL

static const unsigned char ebrima_pixels_small_2bpp[] =
{
    0x00, 0x35, 0x80,
    0x02, 0x55, 0x80,
    0x09, 0x55, 0x80,
    0x09, 0x55, 0x80,
    0x09, 0xe5, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x00, 0x00,
    0x03, 0x55, 0x80,
    0x09, 0x55, 0x50,
    0x35, 0x55, 0x58,
    0x25, 0x72, 0x54,
    0x15, 0x83, 0x57,
    0xd5, 0xc3, 0x57,
    0xd5, 0xc0, 0x56,
    0xd5, 0xc0, 0x56,
    0xd5, 0xc0, 0x56,
    0xd5, 0xc3, 0x57,
    0x15, 0x83, 0x57,
    0x25, 0x72, 0x58,
    0x35, 0x55, 0x58,
    0x09, 0x55, 0x60,
    0x02, 0x55, 0x80,
    0x00, 0x00, 0x00,
    0x02, 0x55, 0x80,
    0x09, 0x55, 0x60,
    0x25, 0x55, 0x58,
    0x15, 0x82, 0x54,
    0xd5, 0xc3, 0x57,
    0xd5, 0xc3, 0x57,
    0xd5, 0x82, 0x56,
    0x25, 0x55, 0x56,
    0x35, 0x55, 0x57,
    0x0d, 0x5b, 0x57,
    0x00, 0x02, 0x54,
    0x38, 0x09, 0x58,
    0x35, 0x55, 0x50,
    0x35, 0x55, 0x70,
    0x39, 0x56, 0x00,
    0x00, 0x00, 0x00,
    0x02, 0x55, 0x80,
    0x09, 0x55, 0x50,
    0x35, 0x55, 0x58,
    0x25, 0x42, 0x54,
    0x25, 0x83, 0x58,
    0x35, 0x42, 0x58,
    0x09, 0x55, 0x60,
    0x0d, 0x55, 0x60,
    0x35, 0x69, 0x58,
    0x15, 0xc0, 0x57,
    0xd5, 0x00, 0x97,
    0xd5, 0x82, 0x57,
    0x25, 0x55, 0x54,
    0x35, 0x55, 0x5c,
    0x02, 0x55, 0x80,
    0x00, 0x00, 0x00,
    0x15, 0x55, 0x56,
    0x15, 0x55, 0x56,
    0x15, 0x55, 0x57,
    0x00, 0x02, 0x58,
    0x00, 0x01, 0x5c,
    0x00, 0x09, 0x60,
    0x00, 0x35, 0x70,
    0x00, 0x25, 0x80,
    0x00, 0xd5, 0xc0,
    0x00, 0x96, 0x00,
    0x00, 0x96, 0x00,
    0x03, 0x57, 0x00,
    0x02, 0x54, 0x00,
    0x02, 0x58, 0x00,
    0x02, 0x58, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0xe5, 0x58,
    0x02, 0x55, 0x58,
    0x0d, 0x55, 0x58,
    0x05, 0x5c, 0x3c,
    0x35, 0x70, 0x00,
    0x25, 0xa5, 0x60,
    0x25, 0x95, 0x58,
    0x15, 0x55, 0x57,
    0x15, 0x72, 0x56,
    0x25, 0x80, 0x96,
    0x25, 0x80, 0x96,
    0x25, 0x72, 0x57,
    0x05, 0x55, 0x54,
    0x0d, 0x55, 0x5c,
    0x03, 0x55, 0x80,
    0x00, 0x00, 0x00,
    0x09, 0x55, 0x5c,
    0x09, 0x55, 0x5c,
    0x05, 0x55, 0x5c,
    0x05, 0x80, 0x00,
    0x05, 0x80, 0x00,
    0x35, 0x80, 0x00,
    0x35, 0x55, 0x80,
    0x35, 0x55, 0x60,
    0x35, 0x55, 0x58,
    0x00, 0x09, 0x58,
    0x00, 0x02, 0x58,
    0x00, 0x09, 0x58,
    0x25, 0x55, 0x5c,
    0x25, 0x55, 0x60,
    0x35, 0x56, 0xc0,
    0x00, 0x00, 0x00,
    0x00, 0x09, 0x60,
    0x00, 0x05, 0x60,
    0x00, 0x25, 0x60,
    0x00, 0xd5, 0x60,
    0x00, 0x55, 0x60,
    0x02, 0x59, 0x60,
    0x09, 0x6d, 0x60,
    0x35, 0x8d, 0x60,
    0xd6, 0x0d, 0x60,
    0x95, 0x55, 0x56,
    0x95, 0x55, 0x56,
    0x95, 0x55, 0x56,
    0x00, 0x0d, 0x60,
    0x00, 0x0d, 0x60,
    0x00, 0x0d, 0x60,
    0x00, 0x00, 0x00,
    0x39, 0x55, 0xc0,
    0x35, 0x55, 0x60,
    0x35, 0x55, 0x50,
    0x00, 0x09, 0x5c,
    0x00, 0x0d, 0x5c,
    0x00, 0x39, 0x60,
    0x0d, 0x55, 0x40,
    0x0d, 0x55, 0xc0,
    0x0d, 0x55, 0x60,
    0x00, 0x09, 0x58,
    0x00, 0x02, 0x58,
    0x28, 0x09, 0x58,
    0x25, 0x55, 0x5c,
    0x25, 0x55, 0x60,
    0x35, 0x56, 0xc0,
    0x00, 0x00, 0x00,
    0x0e, 0x55, 0x80,
    0x35, 0x55, 0x60,
    0x35, 0x55, 0x58,
    0x3b, 0x0d, 0x58,
    0x00, 0x02, 0x58,
    0x00, 0x02, 0x58,
    0x00, 0x09, 0x5c,
    0x00, 0x25, 0x60,
    0x03, 0x55, 0x80,
    0x0d, 0x57, 0x00,
    0x35, 0x60, 0x00,
    0x25, 0x80, 0x00,
    0x15, 0x55, 0x58,
    0xd5, 0x55, 0x58,
    0xd5, 0x55, 0x58,
    0x00, 0x00, 0x00,
    0x00, 0x35, 0x80,
    0x02, 0x55, 0x80,
    0x09, 0x55, 0x80,
    0x09, 0x55, 0x80,
    0x09, 0xe5, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x25, 0x80,
    0x00, 0x00, 0x00,
    0x03, 0x55, 0x80,
    0x09, 0x55, 0x50,
    0x35, 0x55, 0x58,
    0x25, 0x72, 0x54,
    0x15, 0x83, 0x57,
    0xd5, 0xc3, 0x57,
    0xd5, 0xc0, 0x56,
    0xd5, 0xc0, 0x56,
    0xd5, 0xc0, 0x56,
    0xd5, 0xc3, 0x57,
    0x15, 0x83, 0x57,
    0x25, 0x72, 0x58,
    0x35, 0x55, 0x58,
    0x09, 0x55, 0x60,
    0x02, 0x55, 0x80,
    0x00, 0x00, 0x00,
    0x02, 0x55, 0x80,
    0x09, 0x55, 0x60,
    0x25, 0x55, 0x58,
    0x15, 0x82, 0x54,
    0xd5, 0xc3, 0x57,
    0xd5, 0xc3, 0x57,
    0xd5, 0x82, 0x56,
    0x25, 0x55, 0x56,
    0x35, 0x55, 0x57,
    0x0d, 0x5b, 0x57,
    0x00, 0x02, 0x54,
    0x38, 0x09, 0x58,
    0x35, 0x55, 0x50,
    0x35, 0x55, 0x70,
    0x39, 0x56, 0x00
};

static const unsigned long ebrima_palette_small_2bpp[]=
{
    0x000000,   0xffffff,   0xcbcbcb,   0x717171
    //0x000000,   0xffffff,   0x686868,   0x242424
};

const Graphics_Image numbers_ebrima_small_2bpp =
{
    IMAGE_FMT_2BPP_UNCOMP,
    12,
    207,
    4,
    (uint32_t*)ebrima_palette_small_2bpp,
    (uint8_t*)ebrima_pixels_small_2bpp,
};

#endif /* NUMBERS_EBRIMA_SMALL */