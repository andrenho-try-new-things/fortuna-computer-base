#include "fortuna.hh"

#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>

#include "../usb/usb.hh"
#include "../vga/vga.hh"

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

void fortuna::init()
{
    // wait for CORE 1 initialization
    sem_init(&semaphore, 0, 1);
    multicore_launch_core1(core1_entry);
    sem_acquire_blocking(&semaphore);
}
