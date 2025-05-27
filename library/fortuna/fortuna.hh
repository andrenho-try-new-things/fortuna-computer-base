#ifndef FORTUNA_HH
#define FORTUNA_HH

#include "../rtc/rtc.hh"
#include "../usb/usb.hh"
#include "../vga/vga.hh"
#include "../user/user.hh"
#include "../external/external.hh"
#include "hw_config.h"
#include "f_util.h"
#include "ff.h"

namespace fortuna {

struct __attribute__((packed)) Event {
    enum class Type : uint8_t { UserButton, Keyboard, External };
    Type type     : 8;
    union {
        usb::keyboard::Event key;
        uint8_t              buffer[external::BUFFER_RECV_SZ];
    };
};

void init(bool print_welcome);

void add_event(Event const& event);
bool next_event(Event* event);

}

#endif //FORTUNA_HH
