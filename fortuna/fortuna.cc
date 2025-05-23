#include "fortuna.hh"

#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>

static semaphore_t semaphore;

//
// CORE 1 will keep running the devices code and callbacks
//

void core1_entry()
{
    stdio_uart_init();
    printf("==============================================================\n");

    vga::init();
    usb::init();
    user::init();

    sem_release(&semaphore);

    for (;;) {
        usb::step();
    }
}

namespace fortuna {

static constexpr uint8_t MAX_EVENTS = 16;
static Event event_queue[MAX_EVENTS] = {};

void init(bool print_welcome)
{
    // wait for CORE 1 initialization
    sem_init(&semaphore, 0, 1);
    multicore_launch_core1(core1_entry);
    sem_acquire_blocking(&semaphore);

    // add points to corner of the screen, to facilitate monitor calibration
    vga::fb::draw_pixel(0, 0, Color::DarkGreen);
    vga::fb::draw_pixel(639, 0, Color::DarkGreen);
    vga::fb::draw_pixel(0, 479, Color::DarkGreen);
    vga::fb::draw_pixel(639, 479, Color::DarkGreen);

    // initialization message
    if (print_welcome) {
        vga::text::print("Fortuna I/O board version " PROJECT_VERSION "\n(C) Copyright " YEAR ", Andr\x82 Wagner - free hardware/software released under GPLv3\n\n\n\n");

        for (int i = 0; i < 16; ++i)
            for (int x = i * 30 + 50; x < i * 30 + 75; ++x)
                for (int y = 45; y < 70; ++y)
                    vga::fb::draw_pixel(x, y, (Color) i);
    }
}

void add_event(Event const& event)
{
    for (uint8_t i = 0; i < MAX_EVENTS; ++i) {
        if (!event_queue[i].has_data) {
            event_queue[i] = event;
            return;
        }
    }

    // no space
    memmove(event_queue, &event_queue[1], sizeof(Event) * (MAX_EVENTS - 1));
    event_queue[MAX_EVENTS - 1] = event;
}

bool next_event(Event* event)
{
    for (int8_t i = MAX_EVENTS - 1; i >= 0; --i) {
        if (event_queue[i].has_data) {
            *event = event_queue[i];
            event_queue[i].has_data = false;
            return true;
        }
    }

    return false;
}

}
