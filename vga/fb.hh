#ifndef FB_HH
#define FB_HH

#include <stdint.h>

extern unsigned char vga_data_array[];

enum class Color : uint8_t {
    Black = 0b0000,
    Green = 0b0110,
    White = 0b1111,
};

namespace vga::fb {

constexpr uint16_t screen_width() { return 640; }
constexpr uint16_t screen_height() { return 640; }

void draw_pixel(uint16_t x, uint16_t y, Color color);
void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color bg_color, Color fg_color);

}

#endif //FB_HH
