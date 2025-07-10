#include "fortuna.hh"

#include <stdexcept>
#include <string>
using namespace std::string_literals;

#include <SDL3/SDL.h>

static fortuna::Config config;

void fortuna::init(Config const& config_)
{
    config = config_;

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("SDL could not initialize! SDL_Error: "s + SDL_GetError());

    printf("==============================================================\n");

    /*
    // initialize event queue
    queue_init(&event_queue, sizeof(Event), config.event_queue_size);

    rtc::init();
    usb::init();
    user::init();
    */
    vga::init();
    /*
    external::init();
    audio::init();

    sem_release(&semaphore);
    user::init_interupts();
    */
}

void fortuna::finalize()
{

}


bool fortuna::add_event(Event const& event)
{
    return false;  // TODO
}

bool fortuna::next_event(Event *event)
{
    vga::step();
    /*
    usb::step();
    if (config.core1_step_function)
        config.core1_step_function();
    else
        __wfi();
    */

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_EVENT_QUIT) {
            SDL_Quit();
            exit(0);
        }
    }
    return true;
}

uint32_t total_ram()
{
    return 0;  // TODO
}

uint32_t free_ram()
{
    return 0;  // TODO
}
