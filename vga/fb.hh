#ifndef FB_HH
#define FB_HH

#include <stdint.h>

extern unsigned char vga_data_array[];

enum class Color : uint8_t {
    Black = 0b0000,
    White = 0b1111,
};

namespace vga::fb {

void draw_pixel(uint16_t x, uint16_t y, Color color);
void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint8_t x, uint8_t y, Color bg_color, Color fg_color);

}

#endif //FB_HH
