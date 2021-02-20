/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include "numbers_ebrima.h"

static Graphics_Image digit_window =
{
    .bPP = IMAGE_FMT_2BPP_UNCOMP,
    .numColors = 4
};

static Graphics_Rectangle digit_mask;

// ILI9341 low-level methods
extern void ili9486_set_limits(int16_t sX, int16_t sY, int16_t mX, int16_t mY);
extern void ili9486_write_data(int value);
extern void ili9486_write_buffer(uint16_t* values, int32_t windowSize);

extern uint32_t * Graphics_convertPalette(const Graphics_Context *context,
        const Graphics_Image *image);

// Attempt of optimizing image drawing, but no luck
void Graphics_drawImageCustom(const Graphics_Context *context, const Graphics_Image *bitmap, int16_t x, int16_t y) {
    if (bitmap->bPP != IMAGE_FMT_2BPP_UNCOMP) {
        // hack bade to work with uncompressed 2BPP images only
        // revert to default drawImage method
        Graphics_drawImage(context, bitmap, x, y);

        return;
    }

    // Setup draw window
    int mX = x + bitmap->xSize, mY =  y + bitmap->ySize;
    ili9486_set_limits(MAPPED_X(x, y), MAPPED_Y(x, y), MAPPED_X(mX, mY), MAPPED_Y(mX, mY));

    // Cache the pallete values into registers
    register int pallete0 = Graphics_translateColorOnDisplay(context->display, bitmap->pPalette[0]);
    register int pallete1 = Graphics_translateColorOnDisplay(context->display, bitmap->pPalette[1]);
    register int pallete2 = Graphics_translateColorOnDisplay(context->display, bitmap->pPalette[2]);
    register int pallete3 = Graphics_translateColorOnDisplay(context->display, bitmap->pPalette[3]);

    uint8_t step = bitmap->ySize / 4;
    uint8_t* pPixelStart = (uint8_t*)bitmap->pPixel;
    while (pPixelStart < bitmap->pPixel + step) {
        uint8_t* pPixelEnd = pPixelStart + (bitmap->xSize)*step;

        uint8_t* j;

        j = pPixelStart;
        while (j <= pPixelEnd) {
            register uint8_t value = *j;
            switch (value & 0xC0) { // 0b1100_0000
            case 0xC0: // 0b1100_0000
                ili9486_write_data(pallete3);
                break;
            case 0x80: // 0b1000_0000
                ili9486_write_data(pallete2);
                break;
            case 0x40: // 0b0100_0000
                ili9486_write_data(pallete1);
                break;
            default:
                ili9486_write_data(pallete0);
            }
            j += step;
        }

        j = pPixelStart;
        while (j <= pPixelEnd) {
            register uint8_t value = *j;
            switch (value & 0x30) { // 0b0011_0000
            case 0x30: // 0b0011_0000
                ili9486_write_data(pallete3);
                break;
            case 0x20: // 0b0010_0000
                ili9486_write_data(pallete2);
                break;
            case 0x10: // 0b0001_0000
                ili9486_write_data(pallete1);
                break;
            default:
                ili9486_write_data(pallete0);
            }
            j += step;
        }

        j = pPixelStart;
        while (j <= pPixelEnd) {
            register uint8_t value = *j;
            switch (value & 0xC) { // 0b0000_1100
            case 0xC: // 0b0000_1100
                ili9486_write_data(pallete3);
                break;
            case 0x8: // 0b0000_1000
                ili9486_write_data(pallete2);
                break;
            case 0x4: // 0b0000_0100
                ili9486_write_data(pallete1);
                break;
            default:
                ili9486_write_data(pallete0);
            }
            j += step;
        }

        j = pPixelStart;
        while (j <= pPixelEnd) {
            register uint8_t value = *j;
            switch (value & 0x3) { // 0b0000_0011
            case 0x3: // 0b0000_0011
                ili9486_write_data(pallete3);
                break;
            case 0x2: // 0b0000_0010
                ili9486_write_data(pallete2);
                break;
            case 0x1: // 0b0000_0001
                ili9486_write_data(pallete1);
                break;
            default:
                ili9486_write_data(pallete0);
            }
            j += step;
        }

        pPixelStart++;
    }

    // Reset to default draw window
    ili9486_set_limits(0, 0, LCD_X_SIZE - 1, LCD_Y_SIZE - 1);
}

static inline void drawDigit(int x, int y, int d, int w, int h, const Graphics_Image* srcImage) {
    // Sets the address to point to the proper image
    digit_window.xSize = h;
    digit_window.ySize = w;
    digit_window.pPixel = &srcImage->pPixel[d];
    digit_window.pPalette = srcImage->pPalette;

    Graphics_drawImageCustom(&g_sContext, &digit_window, x, y);
}

static inline void drawRoll(int x, int y, int d, int w, int h, int r, const Graphics_Image* srcImage) {
    drawDigit(x - r, y, d - r, w, h + (r << 1), srcImage);
}

//////////////////////////////////////////////
//
//       Ebrima font size 16
//
//////////////////////////////////////////////
#ifdef NUMBERS_EBRIMA_SMALL

extern const Graphics_Image numbers_ebrima_small_2bpp;

void Graphics_drawDigitSmall(int x, int y, int digit){
    digit = CALC_EBRIMA_SMALL_DIGIT_ROW(digit);
    drawDigit(x, y, digit,
              CALC_EBRIMA_SMALL_WIDTH, CALC_EBRIMA_SMALL_HEIGHT,
              &numbers_ebrima_small_2bpp);
}

void Graphics_drawDigitSmallRoll(int x, int y, int digit, int roll) {
    digit = CALC_EBRIMA_SMALL_DIGIT_ROW(digit);
    digit -= CALC_EBRIMA_SMALL_DIGIT_CENTER + (CALC_EBRIMA_SMALL_HEIGHT_SPACED - CALC_EBRIMA_SMALL_HEIGHT) * CALC_EBRIMA_SMALL_BYTES_PER_ROW;
    digit -= CALC_EBRIMA_SMALL_DIGIT_ROLL(roll);

    drawRoll(x, y, digit,
              CALC_EBRIMA_SMALL_WIDTH, CALC_EBRIMA_SMALL_HEIGHT, CALC_EBRIMA_SMALL_ROLL_Y,
              &numbers_ebrima_small_2bpp);
}
#endif /* NUMBERS_EBRIMA_SMALL */

//////////////////////////////////////////////
//
//       Ebrima font size 20
//
//////////////////////////////////////////////
#ifdef NUMBERS_EBRIMA_MEDIUM

extern const Graphics_Image numbers_ebrima_medium_2bpp;
extern const Graphics_Image numbers_ebrima_medium0_2bpp;
void Graphics_clearDigitMedium(int x, int y) {
    digit_mask.xMin = x;
    digit_mask.xMax = x + CALC_EBRIMA_MEDIUM_WIDTH;
    digit_mask.yMin = y;
    digit_mask.yMax = y + CALC_EBRIMA_MEDIUM_HEIGHT;

    Graphics_fillRectangle(&g_sContext, &digit_mask);
}
void Graphics_clearDigitMediumRollSmall(int x, int y) {
    digit_mask.xMin = x;
    digit_mask.xMax = x + CALC_EBRIMA_MEDIUM_WIDTH;
    digit_mask.yMin = y - CALC_EBRIMA_MEDIUM_ROLL_SMALL_X;
    digit_mask.yMax = y - 1;

    Graphics_fillRectangle(&g_sContext, &digit_mask);

    digit_mask.yMin = y + CALC_EBRIMA_MEDIUM_HEIGHT;
    digit_mask.yMax = digit_mask.yMin + CALC_EBRIMA_MEDIUM_ROLL_SMALL_X;
    Graphics_fillRectangle(&g_sContext, &digit_mask);

}

void Graphics_drawDigitMedium(int x, int y, int digit) {
    digit = CALC_EBRIMA_MEDIUM_DIGIT_ROW(digit);

    drawDigit(x, y, digit,
              CALC_EBRIMA_MEDIUM_WIDTH, CALC_EBRIMA_MEDIUM_HEIGHT,
              &numbers_ebrima_medium_2bpp);
}

void Graphics_drawDigitMediumRoll(int x, int y, int digit, int roll) {
    digit = CALC_EBRIMA_MEDIUM_DIGIT_ROW(digit);
    digit -= CALC_EBRIMA_MEDIUM_DIGIT_CENTER + (CALC_EBRIMA_MEDIUM_HEIGHT_SPACED - CALC_EBRIMA_MEDIUM_HEIGHT) * CALC_EBRIMA_MEDIUM_BYTES_PER_ROW;
    digit -= CALC_EBRIMA_MEDIUM_DIGIT_ROLL(roll);

    drawRoll(x, y, digit,
             CALC_EBRIMA_MEDIUM_WIDTH, CALC_EBRIMA_MEDIUM_HEIGHT, CALC_EBRIMA_MEDIUM_ROLL_X,
              &numbers_ebrima_medium_2bpp);
}

void Graphics_drawDigitMediumRollSmall(int x, int y, int digit, int roll) {
    digit = CALC_EBRIMA_MEDIUM_DIGIT_ROW(digit);
    digit -= CALC_EBRIMA_MEDIUM_DIGIT_CENTER + (CALC_EBRIMA_MEDIUM_HEIGHT_SPACED - CALC_EBRIMA_MEDIUM_HEIGHT) * CALC_EBRIMA_MEDIUM_BYTES_PER_ROW;
    digit -= CALC_EBRIMA_MEDIUM_DIGIT_ROLL(roll);

    drawRoll(x, y, digit,
             CALC_EBRIMA_MEDIUM_WIDTH, CALC_EBRIMA_MEDIUM_HEIGHT, CALC_EBRIMA_MEDIUM_ROLL_SMALL_X,
              &numbers_ebrima_medium_2bpp);
}

void Graphics_drawDigitMediumRollZeros(int x, int y, int roll) {
    int digit = CALC_EBRIMA_MEDIUM_ZEROS_DIGIT_ROW;
    digit -= CALC_EBRIMA_MEDIUM_DIGIT_CENTER + (CALC_EBRIMA_MEDIUM_HEIGHT_SPACED - CALC_EBRIMA_MEDIUM_HEIGHT) * CALC_EBRIMA_MEDIUM_BYTES_PER_ROW;
    digit -= CALC_EBRIMA_MEDIUM_DIGIT_ROLL(roll);

    drawRoll(x, y, digit,
             CALC_EBRIMA_MEDIUM_WIDTH, CALC_EBRIMA_MEDIUM_HEIGHT, CALC_EBRIMA_MEDIUM_ROLL_X,
              &numbers_ebrima_medium0_2bpp);
}
#endif /* NUMBERS_EBRIMA_MEDIUM */

#ifdef NUMBERS_EBRIMA_MEDIUM_MEMMAP
extern const int pixel_numbers_medium_memmap;

void Graphics_drawDigitMediumMemmap(int x, int y, int digit) {
    int mX = x + CALC_EBRIMA_MEDIUM_HEIGHT, mY = y + CALC_EBRIMA_MEDIUM_WIDTH;
    ili9486_set_limits(MAPPED_X(x, y), MAPPED_Y(x, y), MAPPED_X(mX, mY), MAPPED_Y(mX, mY));

    digit = CALC_EBRIMA_MEDIUM_DIGIT_MM(digit);
    uint16_t *diplayData = (uint16_t*)(digit + (uint16_t*)&pixel_numbers_medium_memmap);
    y = CALC_EBRIMA_MEDIUM_WIDTH;
    while (y-- > 0) {
        ili9486_write_buffer(diplayData, CALC_EBRIMA_MEDIUM_HEIGHT + 1);
        diplayData += CALC_EBRIMA_MEDIUM_DIGIT_MM_ROW_LEN; // move to same start position on next row
    }
}

void Graphics_drawDigitMediumMemmapRoll(int x, int y, int digit, int roll) {
    const uint8_t h = CALC_EBRIMA_MEDIUM_HEIGHT + (CALC_EBRIMA_MEDIUM_ROLL_X << 1);
    x -= CALC_EBRIMA_MEDIUM_ROLL_X;
    int mY = y + CALC_EBRIMA_MEDIUM_WIDTH, mX = x + h;
    ili9486_set_limits(MAPPED_X(x, y), MAPPED_Y(x, y), MAPPED_X(mX, mY), MAPPED_Y(mX, mY));

    digit = CALC_EBRIMA_MEDIUM_DIGIT_MM(digit) - CALC_EBRIMA_MEDIUM_ROLL_X;
    digit += CALC_EBRIMA_DIGIT_ROLL_PIX(roll, CALC_EBRIMA_MEDIUM_HEIGHT_SPACED);
    uint16_t *diplayData = (uint16_t*)&pixel_numbers_medium_memmap;
    diplayData = (uint16_t*)(digit + diplayData);
    y = CALC_EBRIMA_MEDIUM_WIDTH;
    while (y-- > 0) {
        ili9486_write_buffer(diplayData, h + 1);
        diplayData += CALC_EBRIMA_MEDIUM_DIGIT_MM_ROW_LEN; // move to same start position on next row
    }
}

void Graphics_drawDigitMediumMemmapRollSmall(int x, int y, int digit, int roll) {
    const uint8_t h = CALC_EBRIMA_MEDIUM_HEIGHT + (CALC_EBRIMA_MEDIUM_ROLL_SMALL_X << 1);
    x -= CALC_EBRIMA_MEDIUM_ROLL_SMALL_X;
    int mY = y + CALC_EBRIMA_MEDIUM_WIDTH, mX = x + h;
    ili9486_set_limits(MAPPED_X(x, y), MAPPED_Y(x, y), MAPPED_X(mX, mY), MAPPED_Y(mX, mY));

    digit = CALC_EBRIMA_MEDIUM_DIGIT_MM(digit) - CALC_EBRIMA_MEDIUM_ROLL_SMALL_X;
    digit += CALC_EBRIMA_DIGIT_ROLL_PIX(roll, CALC_EBRIMA_MEDIUM_HEIGHT_SPACED);
    uint16_t *diplayData = (uint16_t*)&pixel_numbers_medium_memmap;
    diplayData = (uint16_t*)(digit + diplayData);
    y = CALC_EBRIMA_MEDIUM_WIDTH;
    while (y-- > 0) {
        ili9486_write_buffer(diplayData, h + 1);
        diplayData += CALC_EBRIMA_MEDIUM_DIGIT_MM_ROW_LEN; // move to same start position on next row
    }
}
#endif /* NUMBERS_EBRIMA_MEDIUM_MEMMAP */


//////////////////////////////////////////////
//
//       Ebrima font size 26
//
//////////////////////////////////////////////
#ifdef NUMBERS_EBRIMA_BIG

extern const Graphics_Image numbers_ebrima_big_2bpp;

void Graphics_drawDigitBig(int x, int y, int digit) {
    digit = CALC_EBRIMA_BIG_DIGIT_ROW(digit);

    drawDigit(x, y, digit,
              CALC_EBRIMA_BIG_WIDTH, CALC_EBRIMA_BIG_HEIGHT,
              &numbers_ebrima_big_2bpp);
}

void Graphics_drawDigitBigRoll(int x, int y, int digit, int roll) {
    digit = CALC_EBRIMA_BIG_DIGIT_ROW(digit);
    digit -= CALC_EBRIMA_BIG_DIGIT_CENTER + (CALC_EBRIMA_BIG_HEIGHT_SPACED - CALC_EBRIMA_BIG_HEIGHT) * CALC_EBRIMA_BIG_BYTES_PER_ROW;
    digit -= CALC_EBRIMA_BIG_DIGIT_ROLL(roll);

    drawRoll(x, y, digit,
             CALC_EBRIMA_BIG_WIDTH, CALC_EBRIMA_BIG_HEIGHT, CALC_EBRIMA_BIG_ROLL_Y,
              &numbers_ebrima_big_2bpp);
}
#endif /* NUMBERS_EBRIMA_BIG */


//////////////////////////////////////////////
//
//       Ebrima font size 34
//
//////////////////////////////////////////////
#ifdef NUMBERS_EBRIMA_HUGE

extern const Graphics_Image numbers_ebrima_huge_2bpp;

void Graphics_drawDigitHuge(int x, int y, int digit) {
    digit = CALC_EBRIMA_HUGE_DIGIT_ROW(digit);

    drawDigit(x, y, digit,
              CALC_EBRIMA_HUGE_WIDTH, CALC_EBRIMA_HUGE_HEIGHT,
              &numbers_ebrima_huge_2bpp);
}

void Graphics_drawDigitHugeRoll(int x, int y, int digit, int roll) {
    digit = CALC_EBRIMA_HUGE_DIGIT_ROW(digit);
    digit -= CALC_EBRIMA_HUGE_DIGIT_CENTER + (CALC_EBRIMA_HUGE_HEIGHT_SPACED - CALC_EBRIMA_HUGE_HEIGHT) * CALC_EBRIMA_HUGE_BYTES_PER_ROW;
    digit -= CALC_EBRIMA_HUGE_DIGIT_ROLL(roll);

    drawRoll(x, y, digit,
             CALC_EBRIMA_HUGE_WIDTH, CALC_EBRIMA_HUGE_HEIGHT, CALC_EBRIMA_HUGE_ROLL_Y,
             &numbers_ebrima_huge_2bpp);
}

#endif /* NUMBERS_EBRIMA_HUGE */
