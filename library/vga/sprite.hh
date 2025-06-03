#ifndef SPRITE_HH
#define SPRITE_HH

struct Image {
    uint8_t w, h;
    uint8_t* data;
};

struct Sprite {
    uint16_t x, y;
    Image* image;
};

#endif //SPRITE_HH
