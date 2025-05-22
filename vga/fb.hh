#ifndef FB_HH
#define FB_HH

#include <stdint.h>

enum class Color : uint8_t {
    Black = 0, NavyBlue, DarkGreen, Blue, Green, SkyBlue, Lime, Cyan,
    Red, Magenta, DarkOrange, Violet, Orange, Pink, Yellow, White,
};

namespace vga::fb {

constexpr uint16_t screen_width() { return 640; }
constexpr uint16_t screen_height() { return 480; }

void clear();
void draw_pixel(uint16_t x, uint16_t y, Color color);
void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color bg_color, Color fg_color);

}

#endif //FB_HH
