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

Color get_pixel_color(uint16_t x, uint16_t y)
{
    const int pixel = ((screen_width * y) + x) ;
    if (pixel & 1)
        return (Color) ((vga_data_array[pixel>>1] >> 4) & 0xf);
    else
        return (Color) (vga_data_array[pixel>>1] & 0xf);
}


static void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color bg_color, Color fg_color)
{
    for (int m = 0; m < n_bytes; ++m) {
        uint8_t v = byte & (1 << (n_bytes - m - 1));
        inline_draw_pixel(x + m, y, v ? fg_color : bg_color);
    }
}

static void draw_from_byte(uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, Color fg_color)
{
    for (int m = 0; m < n_bytes; ++m) {
        uint8_t v = byte & (1 << (n_bytes - m - 1));
        if (v)
            inline_draw_pixel(x + m, y, fg_color);
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
    draw_hline(x, y, x + w, color);
    draw_hline(x, y + h, x + w, color);
    draw_vline(x, y, y + h, color);
    draw_vline(x + w, y, y + h, color);
}

void draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, Color color)
{
    for (int yy = y; yy < (y + h); ++yy)
        draw_hline(x, yy, (x + w), color);
}

void draw_ellipse(uint16_t xc, uint16_t yc, uint16_t rx, uint16_t ry, Color color)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry)
         - (rx * rx * ry)
         + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy) {

        // Print points based on 4-way symmetry
        inline_draw_pixel(x + xc, y + yc, color);
        inline_draw_pixel(-x + xc, y + yc, color);
        inline_draw_pixel(x + xc, -y + yc, color);
        inline_draw_pixel(-x + xc, -y + yc, color);

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0) {

        // printing points based on 4-way symmetry
        inline_draw_pixel(x + xc, y + yc, color);
        inline_draw_pixel(-x + xc, y + yc, color);
        inline_draw_pixel(x + xc, -y + yc, color);
        inline_draw_pixel(-x + xc, -y + yc, color);

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

void draw_ellipse_filled(uint16_t xc, uint16_t yc, uint16_t rx, uint16_t ry, Color color)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry)
         - (rx * rx * ry)
         + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy) {

        // Print points based on 4-way symmetry
        draw_hline(-x + xc, y + yc, x + xc, color);
        draw_hline(-x + xc, -y + yc, x + xc, color);

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0) {

        // printing points based on 4-way symmetry
        draw_hline(-x + xc, y + yc, x + xc, color);
        draw_hline(-x + xc, -y + yc, x + xc, color);

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

void fill_area(uint16_t x, uint16_t y, Color color)
{
    static struct { uint16_t x, y; } stack[256];
    static int top = -1;

    // Get target color and check if fill needed
    Color target = get_pixel_color(x, y);
    if (target == color) return;

    // Push starting point
    stack[++top] = {x, y};

    while (top >= 0) {
        uint16_t px = stack[top].x;
        uint16_t py = stack[top].y;
        top--;

        // Check bounds and color
        if (px >= screen_width || py >= screen_height) continue;
        if (get_pixel_color(px, py) != target) continue;

        // Find left and right bounds of this scanline
        uint16_t left = px, right = px;

        // Scan left
        while (left > 0 && get_pixel_color(left - 1, py) == target) {
            left--;
        }

        // Scan right
        while (right < screen_width - 1 && get_pixel_color(right + 1, py) == target) {
            right++;
        }

        // Fill the line
        draw_hline(left, py, right, color);

        // Add points above and below to stack
        for (uint16_t i = left; i <= right; i++) {
            // Above
            if (py > 0 && top < 255 && get_pixel_color(i, py - 1) == target) {
                stack[++top] = {i, (uint16_t)(py - 1) };
            }
            // Below
            if (py < screen_height - 1 && top < 255 && get_pixel_color(i, py + 1) == target) {
                stack[++top] = {i, (uint16_t) (py + 1) };
            }
        }
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

void draw_image(Image const& image, uint16_t x, uint16_t y, uint8_t framebuffer)
{
    const uint32_t fb_idx = framebuffer * ((screen_width * screen_height) >> 1);

    if ((x & 1) == 0 && !image.has_transparency) {
        for (int m = 0; m < image.h; ++m) {
            const int pixel = ((screen_width * (y + m)) + x) ;
            memcpy(&vga_data_array[fb_idx + (pixel>>1)], &image.data[m * (image.w >> 1)], image.w >> 1);
        }
    } else {
        for (int m = 0; m < image.h; ++m) {
            for (int n = 0; n < image.w; n += 2) {
                const uint32_t idx = ((m * image.w) + n) >> 1;
                Color color1 = (Color) (image.data[idx] & 0xf);
                Color color2 = (Color) ((image.data[idx] >> 4) & 0xf);
                if (image.has_transparency && image.transparent_color != color1)
                    inline_draw_pixel(x + n, y + m, color1, framebuffer);
                if (image.has_transparency && image.transparent_color != color2)
                    inline_draw_pixel(x + n + 1, y + m, color2, framebuffer);
            }
        }
    }
}

void draw_character(uint16_t px, uint16_t py, text::Font const* font, uint8_t ch, Color bg_color, Color fg_color)
{
    for (uint8_t y = 0; y < font->char_height; ++y)
        draw_from_byte(font->pixels(ch - font->first_char, y), font->char_width, px, py + y, bg_color, fg_color);
}

void draw_character(uint16_t px, uint16_t py, text::Font const* font, uint8_t ch, Color fg_color)
{
    for (uint8_t y = 0; y < font->char_height; ++y)
        draw_from_byte(font->pixels(ch - font->first_char, y), font->char_width, px, py + y, fg_color);
}

}
