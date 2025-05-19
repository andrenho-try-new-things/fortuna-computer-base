#include <pico/stdlib.h>
#include "vga/vga.hh"

int main()
{
    vga::init();

    vga::fb::draw_pixel(0, 0, Color::White);
    for (int x = 100; x < 130; ++x)
        for (int y = 100; y < 130; ++y)
            vga::fb::draw_pixel(x, y, Color::White);

    vga::text::print("Hello world!");

    for (;;);
}