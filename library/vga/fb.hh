#ifndef FB_HH
#define FB_HH

#include <cstdint>

#include "sprite.hh"

enum class Color : uint8_t {
    Black = 0, NavyBlue, DarkGreen, Blue, Green, SkyBlue, Lime, Cyan,
    Red, Magenta, DarkOrange, Violet, Orange, Pink, Yellow, White,
};

namespace vga::fb {

void clear(Color color);
void clear_lines(uint16_t y1, uint16_t y2, Color color);
void draw_pixel(uint16_t x, uint16_t y, Color color);
void draw_sprite(SpriteImage const& sprite, uint16_t x, uint16_t y, uint8_t framebuffer=0);
void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color bg_color, Color fg_color);
void move_screen_up(uint16_t lines, Color fill_color);

}

#endif //FB_HH
