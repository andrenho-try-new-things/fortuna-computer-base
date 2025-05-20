#include <pico/stdlib.h>
#include "vga/vga.hh"

int main()
{
    vga::init();

    vga::fb::draw_pixel(0, 0, Color::White);
    vga::fb::draw_pixel(639, 0, Color::White);
    vga::fb::draw_pixel(0, 479, Color::White);
    vga::fb::draw_pixel(639, 479, Color::White);

    vga::text::print(R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas risus felis, efficitur viverra ultrices sit amet, gravida eu diam. Donec egestas massa vel odio gravida pharetra a ac sapien. Mauris sodales vitae odio non sodales. Phasellus hendrerit quis augue ac condimentum. Donec vestibulum quam quis bibendum sollicitudin. Sed eget commodo ipsum, ac sollicitudin lorem. Vestibulum gravida nulla sit amet erat sollicitudin gravida. Aliquam massa risus, tincidunt a eros ac, venenatis consequat sapien. Morbi vel tellus fermentum magna euismod vestibulum. Interdum et malesuada fames ac ante ipsum primis in faucibus. Etiam ullamcorper urna eget ipsum cursus, eget viverra magna imperdiet. Integer nec auctor augue. Curabitur ullamcorper varius ex, sit amet faucibus tellus suscipit in. Etiam efficitur leo at bibendum pretium. Etiam a ex ante. In et viverra mi.

Aliquam feugiat neque sed magna euismod, id venenatis augue faucibus. Proin euismod dolor non urna tincidunt, eu tincidunt ligula faucibus. Suspendisse potenti. Nullam scelerisque nunc enim, a fringilla leo sagittis vitae. Nam nulla turpis, auctor sed mi vitae, porta blandit ligula. Sed imperdiet et libero sit amet efficitur. Donec eget sollicitudin lectus. Donec pulvinar tortor sit amet metus fringilla, a tristique lacus malesuada. Pellentesque id nibh vel sapien convallis bibendum a et velit. Aliquam ornare sodales odio, ut fringilla neque rhoncus vel. Aliquam ut mollis enim. Pellentesque malesuada risus cursus arcu hendrerit porta. Sed nisl metus, tincidunt id mauris luctus, dictum condimentum orci. Mauris accumsan arcu velit, sit amet cursus erat dictum rhoncus.

Pellentesque imperdiet tellus eu nisl viverra ultricies. Proin consequat eros eget augue ultricies, eu gravida libero aliquam. Quisque lacinia, massa non fringilla consequat, nisi sapien hendrerit diam, ac sodales lectus magna non urna. Proin feugiat nunc iaculis est suscipit finibus. Praesent consectetur est quam, nec condimentum nulla euismod et. Fusce sit amet arcu nulla. Fusce feugiat, nisi a semper volutpat, purus orci tristique elit, sit amet finibus purus eros id tortor. Etiam euismod tellus nec sem condimentum, a pellentesque purus feugiat. Maecenas euismod non ex ullamcorper venenatis. Nam ut magna scelerisque, elementum arcu et, auctor velit.

Ut posuere neque interdum justo pellentesque, et condimentum enim scelerisque. Praesent vulputate velit arcu, at fermentum enim cursus id. Mauris nec auctor arcu. Nunc ultrices, libero ac blandit tincidunt, lacus sem ullamcorper augue, eu aliquam nunc enim sed ligula. Maecenas fringilla quis neque at eleifend. Fusce ut risus diam. Nullam mattis et odio eget venenatis. Proin nec risus eu dui laoreet semper in ut est. In hac habitasse platea dictumst.
)");

    for (int i = 0; i < 16; ++i)
        for (int x = i * 30 + 50; x < i * 30 + 75; ++x)
            for (int y = 20; y < 45; ++y)
                vga::fb::draw_pixel(x, y, (Color) i);

    for (;;);
}