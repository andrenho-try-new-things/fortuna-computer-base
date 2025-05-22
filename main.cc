#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>

#include "usb/usb.hh"
#include "vga/vga.hh"

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

    sem_release(&semaphore);

    for (;;) {
        usb::step();
    }
}

//
// CORE 0 will run the user code
//

int main()
{
    // wait for CORE 1 initialization
    sem_init(&semaphore, 0, 1);
    multicore_launch_core1(core1_entry);
    sem_acquire_blocking(&semaphore);

    //
    // USER code below
    //

    vga::text::print("\n\n\n\n\nWelcome to Fortuna I/O board!\n\n");

    vga::fb::draw_pixel(0, 0, Color::White);
    vga::fb::draw_pixel(639, 0, Color::White);
    vga::fb::draw_pixel(0, 479, Color::White);
    vga::fb::draw_pixel(639, 479, Color::White);

    vga::text::print("? ");
    usb::keyboard::subscribe([](usb::keyboard::Event const& e) {
        if (e.pressed && e.chr)
            vga::text::print(e.chr);
        printf("%s '%c' %02X\n", e.pressed ? "pressed" : "released", e.chr, e.hid_key);
    });

    for (int i = 0; i < 16; ++i)
        for (int x = i * 30 + 50; x < i * 30 + 75; ++x)
            for (int y = 20; y < 45; ++y)
                vga::fb::draw_pixel(x, y, (Color) i);

    for (;;);
}