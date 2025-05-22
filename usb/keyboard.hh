#ifndef KEYBOARD_HH
#define KEYBOARD_HH

#include "tusb.h"

namespace usb::keyboard {

struct __attribute__((packed)) Event {
    uint8_t hid_key;   // see tinyusb/src/class/hid/hid.h line 366
    uint8_t chr;
    bool    ctrl     : 1;
    bool    alt      : 1;
    bool    shift    : 1;
    bool    pressed  : 1;
};

void init(uint8_t device_id, uint8_t instance);             // internal
void process_report(hid_keyboard_report_t const *report);   // internal

void subscribe(void(*event)(Event const& event_cb));  // null to unsubcribe

}

#endif //KEYBOARD_HH
