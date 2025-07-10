#include "../vga/vga.hh"

#include <stdexcept>
#include <string>
using namespace std::string_literals;

#include <SDL3/SDL.h>

static SDL_Window* window = nullptr;
static SDL_Renderer* ren = nullptr;
static SDL_Texture* framebuffer = nullptr;
static uint32_t* pixels = nullptr;

void vga::init()
{
    window = SDL_CreateWindow("fortuna-io-board emulator", 640 * 2, 480 * 2, 0);
    if (window == nullptr)
        throw std::runtime_error("Window could not be created! SDL_Error: "s + SDL_GetError());

    ren = SDL_CreateRenderer(window, nullptr);
    if (ren == nullptr)
        throw std::runtime_error("Renderer could not be created! SDL_Error: "s + SDL_GetError());

    framebuffer = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
    if (!framebuffer)
        throw std::runtime_error("Framebuffer could not be created! SDL_Error: "s + SDL_GetError());
    SDL_SetTextureScaleMode(framebuffer, SDL_SCALEMODE_NEAREST);

    pixels = (uint32_t *) calloc(640 * 480, sizeof(uint32_t));
    pixels[(640*10) + 10] = 0xffffffff;
}

void vga::set_mode(Mode mode)
{
    // TODO
}

void vga::step()
{
    SDL_UpdateTexture(framebuffer, nullptr, pixels, 640 * sizeof(uint32_t));

    SDL_RenderClear(ren);
    SDL_RenderTexture(ren, framebuffer, nullptr, nullptr);
    SDL_RenderPresent(ren);
}

void vga::set_sprites(Sprite *sprites, uint16_t sz)
{
    // TODO
}

void vga::update_mouse_position(int8_t x, int8_t y)
{
    // TODO
}
