#ifndef FONT_HH
#define FONT_HH

#include <cstdint>

namespace vga::text {

struct Font {
    Font(uint8_t char_width, uint8_t char_height, uint8_t first_char, uint8_t last_char)
        : char_width(char_width), char_height(char_height), first_char(first_char), last_char(last_char) {}
    virtual ~Font() = default;

    const uint8_t char_width;
    const uint8_t char_height;
    const uint8_t first_char;
    const uint8_t last_char;

    virtual uint8_t pixels(uint8_t chr, uint8_t row) const = 0;
};

}

#endif //FONT_HH
