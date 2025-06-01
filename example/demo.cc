#include <stdio.h>

#include <fortuna.hh>

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

static void ascii_table() {
    vga::text::print("  0123456789ABCDEF\n", false);
    for (uint8_t y = 0; y < 16; ++y) {
        vga::text::printf_noredraw("%X", y);
        vga::text::print(' ', false);
        for (uint8_t x = 0; x < 16; ++x) {
            if (y == 0 && (x == 8 || x == 10))
                vga::text::print(' ', false);
            else
                vga::text::print((y << 4) | x, false);
        }
        vga::text::print('\n', false);
    }
}

static void longtext()
{
    vga::text::print(R"(Morbi volutpat nisi ut pellentesque aliquet. Aenean luctus justo quis lacus ultricies, semper cursus purus commodo. Donec blandit, enim sagittis dictum faucibus, lectus felis tempor nisi, non pharetra nunc purus quis nibh. Curabitur eu mollis tortor, et finibus velit. Suspendisse commodo ac ligula quis laoreet. Suspendisse sit amet blandit purus. In eu lobortis nibh, ut sagittis elit. Maecenas at nisi ut massa scelerisque gravida. Aliquam tempus sagittis felis, quis rhoncus tortor. Sed ornare, massa vel venenatis molestie, metus metus scelerisque metus, vitae bibendum velit est eu lectus.

Curabitur a ultricies est. Pellentesque sit amet pellentesque urna. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut eget pretium sem. Etiam suscipit efficitur massa, vestibulum rutrum leo. Nullam id euismod justo. Duis pulvinar rhoncus porta. Fusce dignissim, dolor eget finibus pretium, arcu ex condimentum mauris, non faucibus tortor odio scelerisque turpis.

Suspendisse potenti. Curabitur laoreet eu orci in ullamcorper. Integer pharetra lorem purus, nec posuere metus iaculis quis. Vivamus a metus a lacus posuere condimentum. Fusce vitae felis commodo, lacinia nulla vel, condimentum odio. Mauris erat ipsum, ultricies non consectetur sit amet, laoreet non libero. Nunc vitae mauris ut urna mattis scelerisque. Etiam et ex pulvinar, cursus nunc a, pharetra eros. Praesent enim nisl, tempus ut fringilla eu, semper a eros. Mauris placerat hendrerit risus at ultrices. Praesent neque leo, egestas eu nisi sed, condimentum porttitor sem.

Aenean vehicula turpis sed risus convallis fringilla. Fusce id mi varius tortor accumsan finibus. Donec accumsan vitae neque vel bibendum. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum suscipit mi a condimentum pretium. In id est lorem. Nulla facilisi. In vehicula non purus ac aliquet. Mauris lacinia magna ac venenatis interdum. Nam lorem velit, semper vitae nunc eu, fermentum blandit lorem. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc dictum eros sed rutrum porttitor. Aliquam at ex euismod, vestibulum ante in, dignissim odio. Nam sit amet aliquet velit, id consectetur nisl. Cras id mauris sed arcu ultrices eleifend.
)");
}

static void table()
{
    using namespace vga::text;
    print('\n', false);

    print(0xc9, false);
    for (uint8_t i = 0; i < 15; ++i) print(0xcd, false);
    print(0xcb, false);
    for (uint8_t i = 0; i < 10; ++i) print(0xcd, false);
    print(0xcb, false);
    for (uint8_t i = 0; i < 10; ++i) print(0xcd, false);
    print(0xbb, false);
    print('\n', false);

    vga::text::printf("%c%-15s%c%-10s%c%-10s%c\n", 0xba, " Country", 0xba, " Exports", 0xba, " Imports", 0xba);

    print(0xc7, false);
    for (uint8_t i = 0; i < 15; ++i) print(0xc4, false);
    print(0xd7, false);
    for (uint8_t i = 0; i < 10; ++i) print(0xc4, false);
    print(0xd7, false);
    for (uint8_t i = 0; i < 10; ++i) print(0xc4, false);
    print(0xb6, false);
    print('\n', false);

    struct {
        Color       color;
        const char* txt;
        float       val1, val2;
    } items[] = {
        { Color::Lime,   "Brazil", 149323.32, 45389.32 },
        { Color::Cyan,   "Argentina", 9343.76, 1776.58 },
        { Color::Orange, "Netherlands", 93482.98, 334.12 },
        { Color::Red,    "U.S.S.R.", -56783.2, 156743.4 },
    };
    for (auto const& item: items) {
        print(0xba, false);
        set_color(Color::Black, item.color);
        printf_noredraw(" %-14s", item.txt);
        set_color(Color::Black, Color::White);
        printf_noredraw("%c%10.2f%c%10.2f%c\n", 0xba, item.val1, 0xba, item.val2, 0xba);
    }

    print(0xc8, false);
    for (uint8_t i = 0; i < 15; ++i) print(0xcd, false);
    print(0xca, false);
    for (uint8_t i = 0; i < 10; ++i) print(0xcd, false);
    print(0xca, false);
    for (uint8_t i = 0; i < 10; ++i) print(0xcd, false);
    print(0xbc, false);
    print("\n\n", false);
}

static void sdcard()
{
    FATFS fs;
    FRESULT fr = f_mount(&fs, "", 1);
    if (FR_OK != fr) {
        vga::text::set_color(Color::Black, Color::Red);
        vga::text::printf("Error mounting SDCard: %s\n", FRESULT_str(fr));
        vga::text::set_color(Color::Black, Color::White);
        f_unmount("");
        return;
    }

    DIR dir;
    fr = f_opendir(&dir, "");
    if (FR_OK != fr) {
        vga::text::set_color(Color::Black, Color::Red);
        vga::text::printf("Error reading root directory: %s\n", FRESULT_str(fr));
        vga::text::set_color(Color::Black, Color::White);
        f_unmount("");
        return;
    }

    FILINFO fno {};
    for (;;) {
        fr = f_readdir(&dir, &fno);
        if (FR_OK != fr) {
            vga::text::set_color(Color::Black, Color::Red);
            vga::text::printf("Error reading file: %s\n", FRESULT_str(fr));
            vga::text::set_color(Color::Black, Color::White);
            f_unmount("");
            return;
        }
        if (fno.fname[0] == 0)
            break;
        if (fno.fattrib & AM_DIR) {            /* Directory */
            vga::text::printf("   <DIR>   %s\n", fno.fname);
        } else {                               /* File */
            vga::text::printf("%10llu %s\n", fno.fsize, fno.fname);
        }
    }
    f_closedir(&dir);

    f_unmount("");
}

static void print_date()
{
    rtc::DateTime d = rtc::get();
    vga::text::printf_noredraw("%04d-%02d-%02d %02d:%02d:%02d", d.year, d.month, d.day, d.hours, d.minutes, d.seconds);
}

static void set_time(const char* cmd)
{
    int yy, mm, dd, hh, nn, ss;
    int n = sscanf(cmd, "settime %d %d %d %d %d %d", &yy, &mm, &dd, &hh, &nn, &ss);
    if (n == 6) {
        rtc::set(rtc::DateTime {
            .year = (uint16_t) yy,
            .month = (uint8_t) mm,
            .day = (uint8_t) dd,
            .hours = (uint8_t) hh,
            .minutes = (uint8_t) nn,
            .seconds = (uint8_t) ss,
        });

    } else {
        vga::text::print("settime YY MM DD HH NN SS\n");
    }
}

void execute_command(const char* cmd)
{
    if (strcmp(cmd, "help") == 0) {
        vga::text::print(R"(audio         play a small tune
ascii         print the ascii table
cls           clear the screen
font NAME     sets the font (fortuna | ibm | vga | toshiba)
led VAL       turns on/off the user panel led (0|1)
longtext      print a longer Lorem Ipsum text
mem           print how much RAM is available
mode [1 | 2]  select VGA mode (640x480 or 320x240 pixels)
sdcard        print the root directory of the SD card
settime DATE  sets the time on the RTC (format: YY MM DD HH NN SS)
switches      shows the current state of the user panel dipswitches
table         print a fancy table
time          prints the current RTC time
)");
    } else if (strcmp(cmd, "audio") == 0) {
        audio::play_music(false);
    } else if (strcmp(cmd, "ascii") == 0) {
        ascii_table();
    } else if (strcmp(cmd, "cls") == 0) {
        vga::text::clear_screen();
        vga::text::set_cursor(0, 0);
    } else if (strcmp(cmd, "longtext") == 0) {
        longtext();
    } else if (strcmp(cmd, "font") == 0) {
        vga::text::print("font [fortuna | ibm | vga | toshiba]\n");
    } else if (strcmp(cmd, "font fortuna") == 0) {
        vga::text::set_font(vga::text::font::Fortuna);
        vga::text::print("Font 'fortuna' selected.\n");
    } else if (strcmp(cmd, "font ibm") == 0) {
        vga::text::set_font(vga::text::font::IBM);
        vga::text::print("Font 'ibm' selected.\n");
    } else if (strcmp(cmd, "font vga") == 0) {
        vga::text::set_font(vga::text::font::VGA);
        vga::text::print("Font 'vga' selected.\n");
    } else if (strcmp(cmd, "font toshiba") == 0) {
        vga::text::set_font(vga::text::font::Toshiba);
        vga::text::print("Font 'vga' selected.\n");
    } else if (strcmp(cmd, "font") == 0) {
        vga::text::print("font NUMBER\n");
    } else if (strcmp(cmd, "sdcard") == 0) {
        sdcard();
    } else if (strcmp(cmd, "led") == 0) {
        vga::text::print("led [0 | 1]\n");
    } else if (strcmp(cmd, "led 0") == 0) {
        user::set_led(false);
    } else if (strcmp(cmd, "led 1") == 0) {
        user::set_led(true);
    } else if (strcmp(cmd, "mode") == 0) {
        vga::text::print("mode [1 | 2]   (640x480 or 320x240 pixels)");
    } else if (strcmp(cmd, "mode 1") == 0) {
        vga::text::clear_screen();
        vga::text::set_cursor(0, 0);
        vga::set_mode(vga::Mode::V_640x480);
        vga::text::print("Mode 1 selected.\n");
    } else if (strcmp(cmd, "mode 2") == 0) {
        vga::text::clear_screen();
        vga::text::set_cursor(0, 0);
        vga::set_mode(vga::Mode::V_320x240);
        vga::text::print("Mode 2 selected.\n");
    } else if (strcmp(cmd, "mem") == 0) {
        vga::text::printf("Memory available: %d kB.\n", free_ram() / 1024);
    } else if (strcmp(cmd, "noise") == 0) {
        audio::play_single_note(audio::Sound { audio::C3, 500 });
    } else if (strcmp(cmd, "switches") == 0) {
        uint8_t s = user::get_dipswitch();
        vga::text::printf("%d%d\n", s & 1, (s >> 1) & 1);
    } else if (strcmp(cmd, "table") == 0) {
        table();
    } else if (strcmp(cmd, "time") == 0) {
        print_date();
        vga::text::print('\n');
    } else if (strncmp(cmd, "settime", 7) == 0) {
        set_time(cmd);
    } else if (cmd[0] != '\0') {
        vga::text::set_color(Color::Black, Color::Red);
        vga::text::print("Error.\n");
        vga::text::set_color(Color::Black, Color::White);
    }
}

// user code will run on CORE 0

void create_music() {
    audio::Sound music[] = {
        { audio::Note::C4, 300 },
        { audio::Note::D4, 300 },
        { audio::Note::E4, 300 },
        { audio::Note::F4, 300 },
        { audio::Note::Pause, 100 },
        { audio::Note::F4, 100 },
        { audio::Note::Pause, 100 },
        { audio::Note::F4, 200 },

        { audio::Note::C4, 300 },
        { audio::Note::D4, 300 },
        { audio::Note::C4, 300 },
        { audio::Note::D4, 300 },
        { audio::Note::Pause, 100 },
        { audio::Note::D4, 100 },
        { audio::Note::Pause, 100 },
        { audio::Note::D4, 300 },

        { audio::Note::C4, 300 },
        { audio::Note::G4, 300 },
        { audio::Note::F4, 300 },
        { audio::Note::E4, 300 },
        { audio::Note::Pause, 100 },
        { audio::Note::E4, 100 },
        { audio::Note::Pause, 100 },
        { audio::Note::E4, 200 },

        { audio::Note::C4, 300 },
        { audio::Note::D4, 300 },
        { audio::Note::E4, 300 },
        { audio::Note::F4, 300 },
        { audio::Note::Pause, 100 },
        { audio::Note::F4, 100 },
        { audio::Note::Pause, 100 },
        { audio::Note::F4, 200 },
    };

    audio::set_music(music, sizeof(music) / sizeof music[0]);
}

int main()
{
    fortuna::init();
    // sleep_ms(3000);

    create_music();

    vga::text::print("Current date/time is ", false); print_date(); vga::text::print(".\n", false);
    vga::text::print("Type 'help' for help.\n\n");

    uint8_t data[128]; for (int i = 0; i < 128; ++i) data[i] = i;
    external::add_response(data, 128);

next_command:
    vga::text::print("? ");

    for (;;) {
        static fortuna::Event e;
        while (fortuna::next_event(&e)) {
            switch (e.type) {
                case fortuna::Event::Type::Keyboard:
                    if (on_key_press(e.key)) {
                        execute_command(command);
                        command[0] = '\0';
                        goto next_command;;
                    }
                    break;
                case fortuna::Event::Type::UserPanel:
                    vga::text::print(" [USER PANEL EVENT: ", false);
                    switch (e.user.button) {
                        case user::Event::PushButton: vga::text::print("push", false); break;
                        case user::Event::Switch0: vga::text::print("switch0", false); break;
                        case user::Event::Switch1: vga::text::print("switch1", false); break;
                    }
                    vga::text::printf(" %d ] ", e.user.new_value);
                    break;
                case fortuna::Event::Type::External:
                    vga::text::print(" [ SPI ", false);
                    for (uint8_t i = 0; i < e.external.sz; ++i)
                        vga::text::printf_noredraw("%02X ", e.external.data[i]);
                    vga::text::print("] ");
                    break;
            }
        }
    }
}
