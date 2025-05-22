#include <stdio.h>

#include "fortuna/fortuna.hh"

// user code will run on CORE 0

static char command[255] = "";

static bool on_key_press(usb::keyboard::Event const& e)
{
    // printf("%s '%c' %02X\n", e.pressed ? "pressed" : "released", e.chr, e.hid_key);

    if (e.pressed) {

        switch (e.hid_key) {
            case HID_KEY_ENTER:
                vga::text::print('\n');
                return true;
            case HID_KEY_BACKSPACE:
                if (command[0] != '\0') {
                    for (uint16_t i = 1; i < 255; ++i) {
                        if (command[i] == '\0') {
                            command[i-1] = '\0';
                            vga::text::print(e.chr);
                            return false;
                        }
                    }
                }
                return false;
        }

        if (e.chr) {
            for (uint16_t i = 0; i < 254; ++i) {
                if (command[i] == '\0') {
                    command[i] = e.chr;
                    command[i+1] = '\0';
                    vga::text::print(e.chr);
                    return false;
                }
            }
            return false;  // command is full
        }

    }

    return false;
}

void execute_command(const char* cmd)
{
    if (strcmp(cmd, "help") == 0) {
        vga::text::print("ascii     boxes    cls     font      longtext\n");
    } else if (strcmp(cmd, "ascii") == 0) {
        for (uint8_t y = 0; y < 16; ++y) {
            for (uint8_t x = 0; x < 16; ++x) {
                if (y == 0 && (x == 8 || x == 10))
                    vga::text::print(' ');
                else
                    vga::text::print((y << 4) | x);
            }
            vga::text::print('\n');
        }
    } else if (strcmp(cmd, "cls") == 0) {
        vga::text::clear_screen();
        vga::text::set_cursor(0, 0);
    } else if (strcmp(cmd, "longtext") == 0) {
        vga::text::print(R"(Morbi volutpat nisi ut pellentesque aliquet. Aenean luctus justo quis lacus ultricies, semper cursus purus commodo. Donec blandit, enim sagittis dictum faucibus, lectus felis tempor nisi, non pharetra nunc purus quis nibh. Curabitur eu mollis tortor, et finibus velit. Suspendisse commodo ac ligula quis laoreet. Suspendisse sit amet blandit purus. In eu lobortis nibh, ut sagittis elit. Maecenas at nisi ut massa scelerisque gravida. Aliquam tempus sagittis felis, quis rhoncus tortor. Sed ornare, massa vel venenatis molestie, metus metus scelerisque metus, vitae bibendum velit est eu lectus.

Curabitur a ultricies est. Pellentesque sit amet pellentesque urna. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut eget pretium sem. Etiam suscipit efficitur massa, vestibulum rutrum leo. Nullam id euismod justo. Duis pulvinar rhoncus porta. Fusce dignissim, dolor eget finibus pretium, arcu ex condimentum mauris, non faucibus tortor odio scelerisque turpis.

Suspendisse potenti. Curabitur laoreet eu orci in ullamcorper. Integer pharetra lorem purus, nec posuere metus iaculis quis. Vivamus a metus a lacus posuere condimentum. Fusce vitae felis commodo, lacinia nulla vel, condimentum odio. Mauris erat ipsum, ultricies non consectetur sit amet, laoreet non libero. Nunc vitae mauris ut urna mattis scelerisque. Etiam et ex pulvinar, cursus nunc a, pharetra eros. Praesent enim nisl, tempus ut fringilla eu, semper a eros. Mauris placerat hendrerit risus at ultrices. Praesent neque leo, egestas eu nisi sed, condimentum porttitor sem.

Aenean vehicula turpis sed risus convallis fringilla. Fusce id mi varius tortor accumsan finibus. Donec accumsan vitae neque vel bibendum. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum suscipit mi a condimentum pretium. In id est lorem. Nulla facilisi. In vehicula non purus ac aliquet. Mauris lacinia magna ac venenatis interdum. Nam lorem velit, semper vitae nunc eu, fermentum blandit lorem. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc dictum eros sed rutrum porttitor. Aliquam at ex euismod, vestibulum ante in, dignissim odio. Nam sit amet aliquet velit, id consectetur nisl. Cras id mauris sed arcu ultrices eleifend.
)");
    } else if (strcmp(cmd, "font") == 0) {
        vga::text::print("font [fortuna | ibm]\n");
    } else if (strcmp(cmd, "font fortuna") == 0) {
        vga::text::set_font(vga::text::font::Fortuna);
        vga::text::print("Font 'fortuna' selected.\n");
    } else if (strcmp(cmd, "font ibm") == 0) {
        vga::text::set_font(vga::text::font::IBM);
        vga::text::print("Font 'ibm' selected.\n");
    } else if (strcmp(cmd, "font") == 0) {
        vga::text::print("font NUMBER\n");
    } else if (cmd[0] != '\0') {
        vga::text::set_color(Color::Black, Color::Red);
        vga::text::print("Error.\n");
        vga::text::set_color(Color::Black, Color::White);
    }
}

int main()
{
    fortuna::init(true);
    vga::text::print("Type 'help' for help.\n");

next_command:
    vga::text::print("? ");

    for (;;) {
        static usb::keyboard::Event e;
        while (usb::keyboard::next_event(&e)) {
            if (on_key_press(e)) {
                execute_command(command);
                command[0] = '\0';
                goto next_command;;
            }
        }
    }
}