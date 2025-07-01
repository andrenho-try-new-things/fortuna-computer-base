#include <SDL3/SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("My SDL3 Project", 640, 480, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(window, nullptr);
    if (ren == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Texture* framebuffer = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
    if (!framebuffer) {
        std::cerr << "Framebuffer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    uint32_t* pixels = (uint32_t *) calloc(640 * 480, sizeof(uint32_t));
    pixels[(640*10) + 10] = 0xffffffff;

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        // Render here (e.g., clear screen, draw)

        SDL_UpdateTexture(framebuffer, nullptr, pixels, 640 * sizeof(uint32_t));

        SDL_RenderClear(ren);
        SDL_RenderTexture(ren, framebuffer, nullptr, nullptr);
        SDL_RenderPresent(ren);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}