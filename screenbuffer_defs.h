#ifndef _SCREEN_BUFFER_DEFS_H_
#define _SCREEN_BUFFER_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct screenbuffer_dev screenbuffer_dev;

typedef void (*screenbuffer_draw_pixel_fptr_t)(screenbuffer_dev *dev, uint16_t x, uint16_t y, int32_t color);

struct screenbuffer_dev
{
    uint16_t screen_width;
    uint16_t screen_heigth;
    uint16_t current_x;
    uint16_t current_y;
    screenbuffer_draw_pixel_fptr_t draw_pixel_fptr;

    GFXfont *font;
    uint8_t font_size_x;
    uint8_t font_size_y;
    bool enable_word_warp;
};

typedef struct
{
    uint16_t x;
    uint16_t y;
} screenbuffer_point;

#endif