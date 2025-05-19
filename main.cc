#include <pico/stdlib.h>
#include "vga/vga.hh"

int main()
{
    vga::init();

    vga::fb::draw_pixel(0, 0, Color::White);
    vga::fb::draw_pixel(10, 10, Color::White);
    vga::fb::draw_pixel(639, 479, Color::White);

    for (;;);
}