#ifndef TEXT_HH
#define TEXT_HH

#include <cstdint>
#include <cstdarg>
#include <utility>

#include "fb.hh"
#include "font.hh"

namespace vga::text {

void init();

void clear_screen();
void set_font(Font const* font);

Font const* default_font();

void print(uint8_t c, bool redraw=true);
void print(const char* text, bool redraw=true);
void printf(const char* fmt, ...);
void printf_noredraw(const char* fmt, ...);

std::pair<uint16_t, uint16_t> get_cursor();
std::pair<Color, Color>       get_color();

void set_color(Color bg_color, Color fg_color);
void set_cursor(uint16_t x, uint16_t y);

void recalculate_matrix_size();

}

#endif //TEXT_HH
