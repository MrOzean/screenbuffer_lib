#ifndef _SCREENBUFFER_H_
#define _SCREENBUFFER_H_

#include <screenbuffer_defs.h>
#include <gfxfont.h>

/*!
 * @brief Set custom font
 *
 * @param[in] dev               Screenbuffer device
 * @param[in] color             GFX compatable font
 * @param[in] font_size_x       Font size by X
 * @param[in] font_size_y       Font size by y
 */
void screenbuffer_set_font(screenbuffer_dev *dev, GFXfont *font, uint8_t font_size_x, uint8_t font_size_y);
/*!
 * @brief Set word wrap
 *
 * @param[in] dev           Screenbuffer device
 * @param[in] enable        Enable
 */
void screenbuffer_set_word_wrap(screenbuffer_dev *dev, bool enable);
/*!
 * @brief Draw single char
 *
 * @param[in] dev           Screenbuffer device
 * @param[in] ch            Char
 * @param[in] color         Char color
 */
void screenbuffer_write_char(screenbuffer_dev *dev, char ch, int32_t color);
/*!
 * @brief Draw string
 *
 * @param[in] dev           Screenbuffer device
 * @param[in] string        String
 * @param[in] color         String color
 */
void screenbuffer_write_string(screenbuffer_dev *dev, char *string, int32_t color);
/*!
 * @brief Move cursor for string/char write
 *
 * @param[in] dev           Screenbuffer device
 * @param[in] x             X coord
 * @param[in] y             Y coord
 */
void screenbuffer_set_cursor(screenbuffer_dev *dev, uint16_t x, uint16_t y);
/*!
 * @brief Draw line
 *
 * @param[in] dev           Screenbuffer device
 * @param[in] x             X coord
 * @param[in] y             Y coord
 */
void screenbuffer_line(screenbuffer_dev *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int32_t color);
void screenbuffer_arc(screenbuffer_dev *dev, uint16_t x, uint16_t y, uint16_t radius, uint16_t start_angle, uint16_t sweep, int32_t color);
void screenbuffer_circle(screenbuffer_dev *dev, uint16_t x, uint16_t y, uint16_t radius, int32_t color);
void screenbuffer_polyline(screenbuffer_dev *dev, screenbuffer_point *point_array, uint16_t point_array_size, int32_t color);
void screenbuffer_rectangle(screenbuffer_dev *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t line_width, int32_t color);
void screenbuffer_fill_rectangle(screenbuffer_dev *dev, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int32_t color);
void screenbuffer_draw_bitmap(screenbuffer_dev *dev, uint16_t x, uint16_t y, uint8_t *bitmap, int16_t bitmap_w, int16_t bitmap_h, int32_t color);
/*!
 * @brief Fill entire screeenbuffer with color
 *
 * @param[in] dev           Screenbuffer device
 * @param[in] color         Color
 */
void screenbuffer_fill_color(screenbuffer_dev *dev, int32_t color);

#endif