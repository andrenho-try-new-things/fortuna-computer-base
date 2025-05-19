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

}

#endif //FB_HH
