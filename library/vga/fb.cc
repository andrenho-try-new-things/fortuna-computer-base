#include "fb.hh"

#include <cmath>
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

static void draw_hline(uint16_t x1, uint16_t y, uint16_t x2, Color color)
{
    if (x1 & 1) {
        inline_draw_pixel(x1, y, color);
        ++x1;
    }
    if (!(x2 & 1)) {
        inline_draw_pixel(x2, y, color);
        --x2;
    }

    const int pixel = ((screen_width * y) + x1);
    memset(&vga_data_array[pixel >> 1], ((uint8_t) color << 4) | (uint8_t) color, (x2 >> 1) - (x1 >> 1) + 1);
}

static void draw_vline(uint16_t x, uint16_t y1, uint16_t y2, Color color)
{
    for (uint16_t y = y1; y <= y2; ++y)
        inline_draw_pixel(x, y, color);
}

void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color color)
{
    if (y1 == y2) {
        draw_hline(x1, y1, x2, color);
    } else if (x1 == x2) {
        draw_vline(x1, y1, y2, color);
    } else {
        auto swap = [](uint16_t a, uint16_t b) { uint16_t t = a; a = b; b = t; };

        int16_t steep = abs(y2 - y1) > abs(x2 - x1);
        if (steep) {
            swap(x1, y1);
            swap(x2, y2);
        }

        if (x1 > x2) {
            swap(x1, x2);
            swap(y1, y2);
        }

        int16_t dx, dy;
        dx = (int16_t) x2 - (int16_t) x1;
        dy = abs((int16_t) y2 - (int16_t) y1);

        int16_t err = dx / 2;
        int16_t ystep;

        if (y1 < y2) {
            ystep = 1;
        } else {
            ystep = -1;
        }

        for (; x1<=x2; x1++) {
            if (steep) {
                inline_draw_pixel(y1, x1, color);
            } else {
                inline_draw_pixel(x1, y1, color);
            }
            err -= dy;
            if (err < 0) {
                y1 += ystep;
                err += dx;
            }
        }
    }
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color)
{

}

void draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color)
{

}

void draw_ellipse(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color)
{

}

void draw_ellipse_filled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color)
{

}

void fill_area(uint16_t x, uint16_t y, Color color)
{

}

void move_screen_up(uint16_t lines, Color fill_color)
{
    uint32_t sz = lines * vga::screen_width / 2;
    uint32_t vga_sz = vga::screen_width * vga::screen_height / 2;
    uint8_t color_data = ((uint8_t) fill_color << 4) | (uint8_t) fill_color;

    memmove(vga_data_array, &vga_data_array[sz], vga_sz - sz);
    memset(&vga_data_array[vga_sz - sz], color_data, sz);
}

void draw_image(Image const& image, uint16_t x, uint16_t y, uint8_t framebuffer)
{
    const uint32_t fb_idx = framebuffer * ((screen_width * screen_height) >> 1);

    if ((x & 1) == 0) {
        for (int m = 0; m < image.h; ++m) {
            const int pixel = ((screen_width * (y + m)) + x) ;
            memcpy(&vga_data_array[fb_idx + (pixel>>1)], &image.data[m * (image.w >> 1)], image.w >> 1);
        }
    } else {
        for (int m = 0; m < image.h; ++m) {
            for (int n = 0; n < image.w; n += 2) {
                const uint32_t idx = ((m * image.w) + n) >> 1;
                inline_draw_pixel(x + n, y + m, (Color) (image.data[idx] & 0xf), framebuffer);
                inline_draw_pixel(x + n + 1, y + m, (Color) ((image.data[idx] >> 4) & 0xf), framebuffer);
            }
        }
    }
}

}
