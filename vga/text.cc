#include "text.hh"

#include "fb.hh"
#include "ibm_font.h"

namespace vga::text {

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;
static Color    fg_color = Color::White;
static Color    bg_color = Color::Black;

static const uint16_t WIDTH = 80;
static const uint16_t HEIGHT = 40;

struct __attribute__((packed)) TextCell {
    uint8_t c = ' ';
    Color   fg_color: 4 = Color::Black;
    Color   bg_color: 4 = Color::White;
    bool    dirty = false;
};

static TextCell cells[WIDTH * HEIGHT] = {};

void init()
{
}

static void add_char(uint8_t c)
{
    cells[cursor_x + (cursor_y * WIDTH)] = TextCell {
        .c = c,
        .fg_color = fg_color,
        .bg_color = bg_color,
        .dirty = true,
    };

    ++cursor_x;

    if (cursor_x >= WIDTH) {
        cursor_x = 0;
        ++cursor_y;
    }

    if (cursor_y >= HEIGHT)
        /* TODO */;
}

static void update()
{
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        if (cells[i].dirty) {
            const int cell_y = i / WIDTH;
            const int cell_x = i - (cell_y * WIDTH);
            const int font_idx = cells[i].c - ibm_font_first_char;
            if (font_idx < 0 || cells[i].c > ibm_font_last_char)
                continue;
            for (uint8_t y = 0; y < ibm_font_height; ++y)
                fb::draw_from_byte(ibm_font_data[font_idx][y], ibm_font_width, cell_x * ibm_font_width, cell_y * ibm_font_height + y, cells[i].bg_color, cells[i].fg_color);
            cells[i].dirty = false;
        }
    }
}

void print(uint8_t c)
{
    add_char(c);
    update();
}

void print(const char* text)
{
    while (*text) {
        add_char(*text);
        text++;
    }
    update();
}

}