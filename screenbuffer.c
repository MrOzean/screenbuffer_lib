#include <screenbuffer.h>
#include <math.h>

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c)
{
    return gfxFont->glyph + c;
}

uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont)
{
    return gfxFont->bitmap;
}

void screenbuffer_set_font(screenbuffer_dev *dev, GFXfont *font, uint8_t font_size_x, uint8_t font_size_y)
{
    dev->font = font;
    dev->font_size_x = font_size_x;
    dev->font_size_y = font_size_y;
}
void screenbuffer_write_char(screenbuffer_dev *dev, char ch, int32_t color)
{
    GFXfont *font = dev->font;

    if (ch > 0x7E)
    {
    }

    ch -= (uint8_t)pgm_read_byte(&font->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(font, ch);
    uint8_t *bitmap = pgm_read_bitmap_ptr(font);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset), yo = pgm_read_byte(
                                                    &glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (dev->font_size_x > 1 || dev->font_size_y > 1)
    {
        xo16 = xo;
        yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    for (yy = 0; yy < h; yy++)
    {
        for (xx = 0; xx < w; xx++)
        {
            if (!(bit++ & 7))
            {
                bits = pgm_read_byte(&bitmap[bo++]);
            }
            if (bits & 0x80)
            {
                if (dev->font_size_x == 1 && dev->font_size_y == 1)
                {
                    dev->draw_pixel_fptr(dev, dev->current_x + xo + xx,
                                         dev->current_y + yo + yy, color);
                }
                else
                {
                    screenbuffer_fill_rectangle(dev,
                                                dev->current_x + (xo16 + xx) * dev->font_size_x,
                                                dev->current_y + (yo16 + yy) * dev->font_size_y,
                                                dev->current_x + (xo16 + xx) * dev->font_size_x + dev->font_size_x,
                                                dev->current_y + (yo16 + yy) * dev->font_size_y + dev->font_size_y,
                                                color);
                }
            }
            bits <<= 1;
        }
    }
    // return ch;
}
void screenbuffer_write_string(screenbuffer_dev *dev, char *str, int32_t color)
{
    // Write until null-byte
    while (*str)
    {

        if (*str == '\n')
        {
            dev->current_x = 0;
            dev->current_y += (int16_t)dev->font_size_y * (uint8_t)pgm_read_byte(dev->font->yAdvance);
        }
        else if (*str != '\r')
        {
            uint8_t first = (uint8_t)dev->font->first;

            GFXglyph *glyph = pgm_read_glyph_ptr(dev->font, *str - first);
            uint8_t w = pgm_read_byte(&glyph->width);
            uint8_t h = pgm_read_byte(&glyph->height);
            if ((w > 0) && (h > 0))
            {                                                        // Is there an associated bitmap?
                int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
                if (dev->enable_word_warp && ((dev->current_x + dev->font_size_x * (xo + w)) > dev->screen_width))
                {
                    dev->current_x = 0;
                    dev->current_y += (int16_t)dev->current_y * (uint8_t)pgm_read_byte(dev->font->yAdvance);
                }
                screenbuffer_write_char(dev, *str, color);
            }
            dev->current_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)dev->font_size_x;
        }

        // Next char
        str++;
    }

    // Everything ok
    // return *str;
}

void screenbuffer_set_word_wrap(screenbuffer_dev *dev, bool enable)
{
    dev->enable_word_warp = enable;
}

void screenbuffer_set_cursor(screenbuffer_dev *dev, uint16_t x, uint16_t y)
{
    dev->current_x = x;
    dev->current_y = y;
}

void screenbuffer_line(screenbuffer_dev *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int32_t color)
{
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;

    dev->draw_pixel_fptr(dev, x2, y2, color);
    while ((x1 != x2) || (y1 != y2))
    {
        dev->draw_pixel_fptr(dev, x1, y1, color);
        error2 = error * 2;
        if (error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        else
        {
            /*nothing to do*/
        }

        if (error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
        else
        {
            /*nothing to do*/
        }
    }
    return;
}

static inline float deg_to_rad(float par_deg)
{
    return par_deg * 3.14 / 180.0;
}
// Normalize degree to [0;360]
static inline uint16_t normalize_to_0_360(uint16_t par_deg)
{
    uint16_t loc_angle;
    if (par_deg <= 360)
    {
        loc_angle = par_deg;
    }
    else
    {
        loc_angle = par_deg % 360;
        loc_angle = ((par_deg != 0) ? par_deg : 360);
    }
    return loc_angle;
}
void screenbuffer_arc(screenbuffer_dev *dev, uint16_t x, uint16_t y, uint16_t radius, uint16_t start_angle, uint16_t sweep, int32_t color)
{
    const int CIRCLE_APPROXIMATION_SEGMENTS = 36;

    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1, xp2;
    uint8_t yp1, yp2;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;

    loc_sweep = normalize_to_0_360(sweep);

    count = (normalize_to_0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;
    while (count < approx_segments)
    {
        rad = deg_to_rad(count * approx_degree);
        xp1 = x + (int8_t)(sin(rad) * radius);
        yp1 = y + (int8_t)(cos(rad) * radius);
        count++;
        if (count != approx_segments)
        {
            rad = deg_to_rad(count * approx_degree);
        }
        else
        {
            rad = deg_to_rad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad) * radius);
        yp2 = y + (int8_t)(cos(rad) * radius);
        screenbuffer_line(dev, xp1, yp1, xp2, yp2, color);
    }
}
void screenbuffer_circle(screenbuffer_dev *dev, uint16_t x, uint16_t y, uint16_t radius, int32_t color)
{
    int32_t px = -radius;
    int32_t py = 0;
    int32_t err = 2 - 2 * radius;
    int32_t e2;

    if (x >= dev->screen_width || y >= dev->screen_heigth)
    {
        return;
    }

    do
    {
        dev->draw_pixel_fptr(dev, x - px, y + py, color);
        dev->draw_pixel_fptr(dev, x + px, y + py, color);
        dev->draw_pixel_fptr(dev, x + px, y - py, color);
        dev->draw_pixel_fptr(dev, x - px, y - py, color);
        e2 = err;
        if (e2 <= py)
        {
            py++;
            err = err + (py * 2 + 1);
            if (-px == py && e2 <= px)
            {
                e2 = 0;
            }
            else
            {
                /*nothing to do*/
            }
        }
        else
        {
            /*nothing to do*/
        }
        if (e2 > px)
        {
            px++;
            err = err + (px * 2 + 1);
        }
        else
        {
            /*nothing to do*/
        }
    } while (px <= 0);

    return;
}
void screenbuffer_polyline(screenbuffer_dev *dev, screenbuffer_point *point_array, uint16_t point_array_size, int32_t color)
{
    uint16_t i;
    if (point_array != 0)
    {
        for (i = 1; i < point_array_size; i++)
        {
            screenbuffer_line(dev, point_array[i - 1].x, point_array[i - 1].y,
                              point_array[i].x, point_array[i].y, color);
        }
    }
}
void screenbuffer_rectangle(screenbuffer_dev *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t line_width, int32_t color)
{
    screenbuffer_line(dev, x1, y1, x2, y1, color);
    screenbuffer_line(dev, x2, y1, x2, y2, color);
    screenbuffer_line(dev, x2, y2, x1, y2, color);
    screenbuffer_line(dev, x1, y2, x1, y1, color);
}
void screenbuffer_fill_rectangle(screenbuffer_dev *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int32_t color)
{
    for (uint16_t x = x1; x <= x2; x += 1)
    {
        for (uint16_t y = y1; y <= y2; y += 1)
        {
            dev->draw_pixel_fptr(dev, x, y, color);
        }
    }
}
void screenbuffer_draw_bitmap(screenbuffer_dev *dev, uint16_t x, uint16_t y, uint8_t *image_array)
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t b = 0;

    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
                b <<= 1;
            else
                b = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
            if (b & 0x80)
                writePixel(x + i, y, color);
        }
    }
}

void screenbuffer_fill_color(screenbuffer_dev *dev, int32_t color)
{
    for (uint16_t x = 0; x < dev->screen_width; x += 1)
    {
        for (uint16_t y = 0; y < dev->screen_heigth; y += 1)
        {
            dev->draw_pixel_fptr(dev, x, y, color);
        }
    }
}