#include "text.hh"

#include <cstring>
#include <pico/time.h>

#include "fb.hh"
#include "font.hh"
#include "fontgen/ibm_font.hh"
#include "fontgen/fortuna_font.hh"

static fortuna_font default_font;
static ibm_font secondary_font;

namespace vga::text {

static Font*   font_data[] { &default_font, &secondary_font };
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
static constexpr uint8_t V_BORDER = 10;

struct __attribute__((packed)) TextCell {
    uint8_t c = ' ';
    Color   fg_color: 4 = Color::Black;
    Color   bg_color: 4 = Color::White;
    bool    dirty = false;
};

static TextCell* cells;

static void line_feed()
{
    for (int i = 0; i < columns * rows; ++i) {
        if (i < (columns * (rows - 1))) {
            cells[i] = cells[i + columns];
            cells[i].dirty = true;
        } else {
            cells[i] = TextCell { ' ', fg_color, bg_color, true };
        }
    }
}

static void add_char(uint8_t c)
{
    if (c == 10) {
        add_char(' ');
        while (cursor_x != 0)
            add_char(' ');
    } else {
        cells[cursor_x + (cursor_y * columns)] = TextCell {
            .c = c,
            .fg_color = fg_color,
            .bg_color = bg_color,
            .dirty = true,
        };

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
}

static void redraw()
{
    Font* font = font_data[current_font];
    if (!font)
        return;

    const int w_border = (fb::screen_width()) / 2 - (columns * font->char_width / 2);

    for (int i = 0; i < columns * rows; ++i) {
        const int cell_y = i / columns;
        const int cell_x = i - (cell_y * columns);

        if (cells[i].dirty) {
            const int font_idx = cells[i].c - font->first_char;
            if (font_idx < 0 || cells[i].c > font->last_char)
                continue;
            for (uint8_t y = 0; y < font->char_height; ++y)
                fb::draw_from_byte(font->pixels(font_idx, y), font->char_width, cell_x * font->char_width + w_border, cell_y * font->char_height + y + V_BORDER, cells[i].bg_color, cells[i].fg_color);
            cells[i].dirty = false;
        }

        if (cell_x == cursor_x && cell_y == cursor_y) {
            for (uint8_t y = 0; y < font->char_height; ++y)
                fb::draw_from_byte(cursor_is_on ? font->pixels(CURSOR_CHAR - font->first_char, y) : 0x0,
                    font->char_width, cell_x * font->char_width + w_border, cell_y * font->char_height + y + V_BORDER, Color::Black, Color::Green);
        }
    }
}

void init()
{
    set_font(font::Fortuna);

    add_repeating_timer_ms(100, [](auto*) {
        --cursor_counter;
        if (cursor_counter == 0) {
            cursor_is_on = !cursor_is_on;
            cursor_counter = CURSOR_RESET;
            redraw();
        }
        return true;
    }, nullptr, &timer);
}

void clear_screen()
{
    for (size_t i = 0; i < rows * columns; ++i)
        cells[i] = TextCell { ' ', fg_color, bg_color, true };
    redraw();
}

void set_font(font f)
{
    if ((uint8_t) f < sizeof(font_data) / sizeof(font_data[0])) {
        clear_screen();
        delete cells;
        current_font = (uint8_t) f;
        Font* font = font_data[(uint8_t) f];
        if (font) {
            columns = fb::screen_width() / font->char_width;
            if (columns > 80)
                columns = 80;
            rows = (fb::screen_height() - 2 * V_BORDER) / font->char_height;
            cells = new TextCell[columns * rows];
            clear_screen();
        }
    }
}

void print(uint8_t c)
{
    add_char(c);
    redraw();
}

void print(const char* text)
{
    while (*text) {
        add_char(*text);
        text++;
    }
    redraw();
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
    cursor_x = MAX(x, 639);
    cursor_y = MAX(y, 479);
}

}
