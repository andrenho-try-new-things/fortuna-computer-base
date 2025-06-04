#include "text.hh"

#include <cstring>
#include <cstdio>
#include <pico/time.h>

#include "fb.hh"
#include "font.hh"
#include "vga.hh"
#include "fontgen/fortuna_font.hh"

namespace vga::text {

static const fortuna_font font_default;
static Font const* current_font = default_font();

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
    const int w_border = (vga::screen_width) / 2 - (columns * current_font->char_width / 2);
    return { w_border + (cell_x * current_font->char_width), V_BORDER + (cell_y * current_font->char_height) };
}

static void line_feed()
{
    fb::move_screen_up(current_font->char_height, bg_color);
    fb::clear_lines(0, V_BORDER, bg_color);
}

static void draw_cursor()
{
    auto [px, py] = cell_pos(cursor_x, cursor_y);
    fb::draw_character(px, py, current_font, CURSOR_CHAR, bg_color, cursor_is_on ? Color::Lime : bg_color);
}

void init()
{
    set_font(default_font());

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

void recalculate_matrix_size()
{
    columns = (vga::screen_width / current_font->char_width) - 1;
    if (columns > 80)
        columns = 80;
    rows = (vga::screen_height - 2 * V_BORDER) / current_font->char_height;
}

void set_font(Font const* f)
{
    clear_screen();
    fb::clear(bg_color);
    current_font = f;
    recalculate_matrix_size();
    clear_screen();
    set_cursor(0, 0);
}

Font const* default_font()
{
    return &font_default;
}

void print(uint8_t c, bool redraw)
{
    auto [px, py] = cell_pos(cursor_x, cursor_y);

    if (c == 10) {
        cursor_is_on = false;
        draw_cursor();
        cursor_x = 0;
        ++cursor_y;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            --cursor_x;
            fb::draw_character(px, py, current_font, ' ', bg_color, fg_color);
        }
    } else {
        fb::draw_character(px, py, current_font, c, bg_color, fg_color);
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
