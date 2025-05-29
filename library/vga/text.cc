#include "text.hh"

#include <cstring>
#include <cstdio>
#include <pico/time.h>

#include "fb.hh"
#include "font.hh"
#include "fontgen/ibm_font.hh"
#include "fontgen/fortuna_font.hh"
#include "fontgen/vga_font.hh"
#include "fontgen/toshiba_font.hh"

static fortuna_font default_font;
static ibm_font font_ibm;
static vga_font font_vga;
static toshiba_font font_toshiba;

namespace vga::text {

static Font*   font_data[] { &default_font, &font_ibm, &font_vga, &font_toshiba };
static uint8_t current_font = 0;

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;
static Color    fg_color = Color::White;
static Color    bg_color = Color::Black;

static repeating_timer_t timer;
static bool              cursor_is_on = true;
static constexpr uint8_t CURSOR_RESET = 6;
static uint8_t           cursor_counter = CURSOR_RESET;

static uint16_t columns = 80;
static uint16_t rows = 25;

static constexpr uint8_t CURSOR_CHAR = 127;
static constexpr uint8_t MISSING_CHAR = '?';
static constexpr uint8_t V_BORDER = 10;

static std::pair<uint16_t, uint16_t> cell_pos(uint16_t cell_x, uint16_t cell_y)
{
    Font* font = font_data[current_font];
    if (!font)
        return { 0, 0 };

    const int w_border = (fb::screen_width()) / 2 - (columns * font->char_width / 2);

    return { w_border + (cell_x * font->char_width), V_BORDER + (cell_y * font->char_height) };
}

static void line_feed()
{
    Font* font = font_data[current_font];
    if (!font)
        return;

    fb::move_screen_up(font->char_height, bg_color);
    fb::clear_lines(0, V_BORDER, bg_color);
}

static void draw_cursor()
{
    auto [px, py] = cell_pos(cursor_x, cursor_y);
    Font* font = font_data[current_font];
    const int font_idx = CURSOR_CHAR - font->first_char;

    for (uint8_t y = 0; y < font->char_height; ++y)
        fb::draw_from_byte(font->pixels(font_idx, y), font->char_width, px, py + y, bg_color, cursor_is_on ? Color::Lime : bg_color);
}

void init()
{
    set_font(font::Fortuna);

    add_repeating_timer_ms(100, [](auto*) {
        --cursor_counter;
        if (cursor_counter == 0) {
            cursor_is_on = !cursor_is_on;
            cursor_counter = CURSOR_RESET;
            draw_cursor();
        }
        return true;
    }, nullptr, &timer);
}

void clear_screen()
{
    fb::clear(bg_color);
}

void set_font(font f)
{
    if ((uint8_t) f < sizeof(font_data) / sizeof(font_data[0])) {
        clear_screen();
        fb::clear(bg_color);
        current_font = (uint8_t) f;
        Font* font = font_data[(uint8_t) f];
        if (font) {
            columns = fb::screen_width() / font->char_width;
            if (columns > 80)
                columns = 80;
            rows = (fb::screen_height() - 2 * V_BORDER) / font->char_height;
            clear_screen();
            set_cursor(0, 0);
        }
    }
}

void print(uint8_t c, bool redraw)
{
    auto [px, py] = cell_pos(cursor_x, cursor_y);
    Font* font = font_data[current_font];

    if (c == 10) {
        cursor_is_on = false;
        draw_cursor();
        cursor_x = 0;
        ++cursor_y;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            --cursor_x;
            for (uint8_t y = 0; y < font->char_height; ++y)
                fb::draw_from_byte(font->pixels(' ' - font->first_char, y), font->char_width, px, py + y, bg_color, fg_color);
        }
    } else {
        int font_idx = c - font->first_char;
        if (font_idx < 0 || c > font->last_char)
            font_idx = MISSING_CHAR - font->first_char;
        for (uint8_t y = 0; y < font->char_height; ++y)
            fb::draw_from_byte(font->pixels(font_idx, y), font->char_width, px, py + y, bg_color, fg_color);

        ++cursor_x;
    }

    if (cursor_x >= columns) {
        cursor_x = 0;
        ++cursor_y;
    }

    if (cursor_y >= rows) {
        line_feed();
        cursor_y = rows - 1;
    }

    cursor_counter = CURSOR_RESET;
    cursor_is_on = true;
    draw_cursor();
}

void print(const char* text, bool redraw)
{
    while (*text) {
        print(*text);
        text++;
    }
}

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int sz = vsnprintf(nullptr, 0, fmt, args);
    char buf[sz + 1] = {0};
    vsnprintf(buf, sz + 1, fmt, args);
    print(buf, true);
    va_end(args);
}

void printf_noredraw(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int sz = vsnprintf(nullptr, 0, fmt, args);
    char buf[sz + 1] = {0};
    vsnprintf(buf, sz + 1, fmt, args);
    print(buf, false);
    va_end(args);
}

std::pair<uint16_t, uint16_t> get_cursor()
{
    return { cursor_x, cursor_y };
}

std::pair<Color, Color> get_color()
{
    return { bg_color, fg_color };
}

void set_color(Color bg_color_, Color fg_color_)
{
    bg_color = bg_color_;
    fg_color = fg_color_;
}

void set_cursor(uint16_t x, uint16_t y)
{
    cursor_x = MIN(x, 639);
    cursor_y = MIN(y, 479);
}

}
