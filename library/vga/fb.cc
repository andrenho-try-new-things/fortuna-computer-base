#include "fb.hh"

#include <cstring>

#include "vga.hh"

#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

namespace vga::fb {

void clear()
{
    memset(vga_data_array, 0, screen_width() * screen_height() / 2);
}

void draw_pixel(uint16_t x, uint16_t y, Color color)
{
    // Range checks (640x480 display)
    if((x > 639) | (x < 0) | (y > 479) | (y < 0) ) return;

    // Which pixel is it?
    const int pixel = ((640 * y) + x) ;

    // Is this pixel stored in the first 4 bits
    // of the vga data array index, or the second
    // 4 bits? Check, then mask.
    if (pixel & 1)
        vga_data_array[pixel>>1] = (vga_data_array[pixel>>1] & TOPMASK) | ((uint8_t) color << 4) ;
    else
        vga_data_array[pixel>>1] = (vga_data_array[pixel>>1] & BOTTOMMASK) | ((uint8_t) color) ;
}

void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color bg_color, Color fg_color)
{
    for (int m = 0; m < n_bytes; ++m) {
        uint8_t v = byte & (1 << (n_bytes - m - 1));
        draw_pixel(x + m, y, v ? fg_color : bg_color);
    }
}

}
