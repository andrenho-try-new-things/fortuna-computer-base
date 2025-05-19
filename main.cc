#include <pico/stdlib.h>
#include "vga/vga.hh"

int main()
{
    vga::init();

    for (;;);
}