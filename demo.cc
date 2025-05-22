#include <stdio.h>

#include "fortuna/fortuna.hh"

// user code will run on CORE 0

int main()
{
    fortuna::init();

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