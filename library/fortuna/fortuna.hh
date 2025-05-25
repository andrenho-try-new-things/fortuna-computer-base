#ifndef FORTUNA_HH
#define FORTUNA_HH

#include "../rtc/rtc.hh"
#include "../usb/usb.hh"
#include "../vga/vga.hh"
#include "../user/user.hh"
#include "hw_config.h"
#include "f_util.h"
#include "ff.h"

namespace fortuna {

struct __attribute__((packed)) Event {
    enum class Type { UserButton, Keyboard };
    Type type     : 7;
    bool has_data : 1;
    union {
        usb::keyboard::Event key;
    };
};

void init(bool print_welcome);

void add_event(Event const& event);
bool next_event(Event* event);

}

#endif //FORTUNA_HH
