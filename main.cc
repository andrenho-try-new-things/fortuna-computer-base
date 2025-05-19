#include <pico/stdlib.h>
#include "vga/vga.hh"

int main()
{
    vga::init();

    vga::fb::draw_pixel(0, 0, Color::White);
    vga::fb::draw_pixel(639, 0, Color::White);
    vga::fb::draw_pixel(0, 479, Color::White);
    vga::fb::draw_pixel(639, 479, Color::White);

    for (int x = 600; x < 630; ++x)
        for (int y = 100; y < 130; ++y)
            vga::fb::draw_pixel(x, y, Color::White);

    vga::text::print(R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id sem sit amet massa faucibus posuere. Nulla placerat purus eget eros maximus dignissim. Proin aliquam nisl a aliquam semper. Ut condimentum semper diam, ut consequat velit convallis sed. Nunc laoreet finibus orci. Pellentesque enim ipsum, tincidunt sit amet efficitur sed, lacinia eget lorem. Donec varius eros ac neque vulputate mattis. Donec consequat sollicitudin augue, sed vulputate ante tempor vitae. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Nulla pellentesque sollicitudin nulla in varius.

Morbi dictum dictum cursus. Nam consequat venenatis aliquam. In luctus aliquam mauris et dapibus. Pellentesque risus mi, commodo a interdum a, tempor pharetra quam. Mauris interdum pharetra velit nec ultricies. Praesent pellentesque lorem in semper rutrum. Maecenas sollicitudin id neque sit amet pharetra. Duis eget neque nec justo feugiat condimentum in vel dui. Nam dignissim mollis molestie. Vivamus fringilla libero condimentum dolor consectetur, id vestibulum ligula feugiat. Sed efficitur nec nibh eget vulputate. Etiam tincidunt varius tellus, quis viverra risus gravida id. Ut et elit sagittis, porttitor magna vel, finibus odio.)");

    for (;;);
}