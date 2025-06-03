#ifndef SPRITE_HH
#define SPRITE_HH

#include "color.hh"

struct __attribute__((packed)) Image {
    uint8_t  w, h;
    bool     has_transparency : 1;
    Color    transparent_color: 4;
    uint8_t* data;

    Image(uint8_t w, uint8_t h, uint8_t* data) : w(w), h(h), has_transparency(false), transparent_color(Color::Black), data(data) {}
    Image(uint8_t w, uint8_t h, Color transparent_color, uint8_t* data)
        : w(w), h(h), has_transparency(true), transparent_color(transparent_color), data(data) {}
};

struct Sprite {
    uint16_t x, y;
    Image* image;
};

#endif //SPRITE_HH
