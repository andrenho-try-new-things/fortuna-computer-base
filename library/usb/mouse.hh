#ifndef MOUSE_HH
#define MOUSE_HH

#include <cstdint>

#include "tusb.h"

namespace usb::mouse {

namespace button {
    constexpr uint8_t Left = 0b1;
    constexpr uint8_t Right = 0b10;
    constexpr uint8_t Middle = 0b100;
};

struct __attribute__((packed)) Event {
    int8_t  x, y;
    int8_t  wheel;
    uint8_t buttons;
};

void init(uint8_t device_id_, uint8_t instance_);
void process_report(hid_mouse_report_t const *report);   // internal

}

#endif //MOUSE_HH
