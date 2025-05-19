#include "fb.hh"

#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

namespace vga::fb {

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

}
