#include "fb.hh"

#include <cstring>

#include "vga.hh"

#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

namespace vga::fb {

void clear(Color color)
{
    uint8_t color_data = ((uint8_t) color << 4) | (uint8_t) color;
    memset(vga_data_array, color_data, screen_width() * screen_height() / 2);
}

void clear_lines(uint16_t y1, uint16_t y2, Color color)
{
    if (y1 > y2)
        return;

    uint8_t color_data = ((uint8_t) color << 4) | (uint8_t) color;
    uint32_t start = y1 * screen_width() / 2;
    uint32_t end = y2 * screen_width() / 2;
    memset(&vga_data_array[start], color_data, end - start);
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

void move_screen_up(uint16_t lines, Color fill_color)
{
    uint32_t sz = lines * screen_width() / 2;
    uint32_t vga_sz = screen_width() * screen_height() / 2;
    uint8_t color_data = ((uint8_t) fill_color << 4) | (uint8_t) fill_color;

    memmove(vga_data_array, &vga_data_array[sz], vga_sz - sz);
    memset(&vga_data_array[vga_sz - sz], color_data, sz);
}

}
