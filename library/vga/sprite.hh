#ifndef SPRITE_HH
#define SPRITE_HH

struct SpriteImage {
    uint8_t w, h;
    uint8_t* data;
};

struct Sprite {
    uint16_t x, y;
    SpriteImage* image;
};

#endif //SPRITE_HH
