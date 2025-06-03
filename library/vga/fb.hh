#ifndef FB_HH
#define FB_HH

#include <cstdint>

#include "color.hh"
#include "font.hh"
#include "sprite.hh"

namespace vga::fb {

void clear(Color color);
void clear_lines(uint16_t y1, uint16_t y2, Color color);

void draw_pixel(uint16_t x, uint16_t y, Color color);
Color get_pixel_color(uint16_t x, uint16_t y);

void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color);
void draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);
void draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);
void draw_ellipse(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);
void draw_ellipse_filled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color);

void draw_character(uint16_t x, uint16_t y, text::Font const* font, uint8_t ch, Color bg_color, Color fg_color);
void draw_character(uint16_t x, uint16_t y, text::Font const* font, uint8_t ch, Color fg_color);

void fill_area(uint16_t x, uint16_t y, Color color);

void draw_image(Image const& image, uint16_t x, uint16_t y, uint8_t framebuffer=0);

void move_screen_up(uint16_t lines, Color fill_color);

}

#endif //FB_HH
