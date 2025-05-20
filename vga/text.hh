#ifndef TEXT_HH
#define TEXT_HH

#include <cstdint>
#include <utility>

#include "fb.hh"
#include "font.hh"

namespace vga::text {

void init();

void clear_screen();

void print(uint8_t c);
void print(const char* text);

std::pair<uint16_t, uint16_t> get_cursor();
std::pair<Color, Color>       get_color();

void set_color(Color bg_color, Color fg_color);
void set_cursor(uint16_t x, uint16_t y);

}

#endif //TEXT_HH
