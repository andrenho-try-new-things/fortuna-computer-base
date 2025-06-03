#include "fb.hh"

#include <cstring>

#include "vga.hh"

#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

namespace vga::fb {

void clear(Color color)
{
    uint8_t color_data = ((uint8_t) color << 4) | (uint8_t) color;
    memset(vga_data_array, color_data, vga::screen_width * vga::screen_height / 2);
}

void clear_lines(uint16_t y1, uint16_t y2, Color color)
{
    if (y1 > y2)
        return;

    uint8_t color_data = ((uint8_t) color << 4) | (uint8_t) color;
    uint32_t start = y1 * vga::screen_width / 2;
    uint32_t end = y2 * vga::screen_width / 2;
    memset(&vga_data_array[start], color_data, end - start);
}

static void __attribute__((always_inline)) inline inline_draw_pixel(uint16_t x, uint16_t y, Color color, uint8_t framebuffer=0)
{
    const uint32_t fb_idx = framebuffer * ((screen_width * screen_height) >> 1);

    // Which pixel is it?
    const int pixel = ((screen_width * y) + x) ;

    // Is this pixel stored in the first 4 bits
    // of the vga data array index, or the second
    // 4 bits? Check, then mask.
    if (pixel & 1)
        vga_data_array[fb_idx + (pixel>>1)] = (vga_data_array[fb_idx + (pixel>>1)] & TOPMASK) | ((uint8_t) color << 4) ;
    else
        vga_data_array[fb_idx + (pixel>>1)] = (vga_data_array[fb_idx + (pixel>>1)] & BOTTOMMASK) | ((uint8_t) color) ;
}


void draw_pixel(uint16_t x, uint16_t y, Color color)
{
    if((x > vga::screen_width - 1) | (x < 0) | (y > screen_height - 1) | (y < 0) ) return;
    inline_draw_pixel(x, y, color);
}


void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color bg_color, Color fg_color)
{
    for (int m = 0; m < n_bytes; ++m) {
        uint8_t v = byte & (1 << (n_bytes - m - 1));
        inline_draw_pixel(x + m, y, v ? fg_color : bg_color);
    }
}

void move_screen_up(uint16_t lines, Color fill_color)
{
    uint32_t sz = lines * vga::screen_width / 2;
    uint32_t vga_sz = vga::screen_width * vga::screen_height / 2;
    uint8_t color_data = ((uint8_t) fill_color << 4) | (uint8_t) fill_color;

    memmove(vga_data_array, &vga_data_array[sz], vga_sz - sz);
    memset(&vga_data_array[vga_sz - sz], color_data, sz);
}

void draw_sprite(SpriteImage const& sprite, uint16_t x, uint16_t y, uint8_t framebuffer)
{
    const uint32_t fb_idx = framebuffer * ((screen_width * screen_height) >> 1);

    if ((x & 1) == 0) {
        for (int m = 0; m < sprite.h; ++m) {
            const int pixel = ((screen_width * (y + m)) + x) ;
            memcpy(&vga_data_array[fb_idx + (pixel>>1)], &sprite.data[m * (sprite.w >> 1)], sprite.w >> 1);
        }
    } else {
        for (int m = 0; m < sprite.h; ++m) {
            for (int n = 0; n < sprite.w; n += 2) {
                const uint32_t idx = ((m * sprite.w) + n) >> 1;
                inline_draw_pixel(x + n, y + m, (Color) (sprite.data[idx] & 0xf), framebuffer);
                inline_draw_pixel(x + n + 1, y + m, (Color) ((sprite.data[idx] >> 4) & 0xf), framebuffer);
            }
        }
    }
}

}
