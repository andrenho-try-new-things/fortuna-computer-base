#ifndef FORTUNA_HH
#define FORTUNA_HH

#include "../audio/audio.hh"
#include "../rtc/rtc.hh"
#include "../usb/usb.hh"
#include "../vga/vga.hh"
#include "../user/user.hh"
#include "../external/external.hh"
#include "hw_config.h"
#include "f_util.h"
#include "ff.h"

namespace fortuna {

struct Config {
    bool     print_welcome = true;
    uint16_t event_queue_size = 64;
    void     (*core1_step_function)() = nullptr;   // function to run on core1 repetedly in parallel to device management
};

struct __attribute__((packed)) Event {
    enum class Type : uint8_t { UserPanel, Keyboard, External };
    Type type     : 8;
    union {
        user::Event          user;
        usb::keyboard::Event key;
        external::Buffer     external;
    };
};

void init(Config const& config={});

bool add_event(Event const& event);
bool next_event(Event* event);

}

uint32_t total_ram();
uint32_t free_ram();

#endif //FORTUNA_HH
