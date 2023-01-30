# ScreenBuffer library

Inspired by [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) pure C11 dependency free implementation of drawing basic  primitives and text printing on any screen

Supported:

+ Lines and Polyline's
+ Arcs
+ Circles
+ Rectangles
+ Clear/fill by color
+ Bitmap
+ Text, using Adafruit GFX compatable fonts

## Usage

```c
#include "screenbuffer.h"    // include library
#include "fonts/FreeMono8.h" // include font

#define COLOR_BLACK ...
...

screenbuffer_dev screenbuff = {};

void draw_pixel(screenbuffer_dev *dev, uint16_t x, uint16_t y, int32_t color)
{
    // any user defined draw pixel implementation
    ...
}

void init() {
    screenbuff.draw_pixel_fptr = draw_pixel; // pointer to draw pixel function
    screenbuff.enable_word_warp = true;
    screenbuff.font = &FreeMono8pt8b; // pointer to font, can be replaced using screenbuffer_set_font()
    screenbuff.font_size_x = 1;
    screenbuff.font_size_y = 1;
    screenbuff.screen_heigth = 800;
    screenbuff.screen_width = 600;
}

int main() {
    init();
    ... 

    screenbuffer_line(&screenbuff, 10, 10, 50,50, COLOR_BLACK);
    screenbuffer_circle(&screenbuff, 30, 30, 30, COLOR_BLACK);

    screenbuffer_set_cursor(&screenbuff, 50,10);
    screenbuffer_write_string(&screenbuff, "Hello!", COLOR_BLACK);

    update_screen(...);
}

```
