#ifndef MOUSE_HH
#define MOUSE_HH

#include <cstdint>

#include "tusb.h"

namespace usb::mouse {

void init(uint8_t device_id_, uint8_t instance_);
void process_report(hid_mouse_report_t const *report);   // internal

}

#endif //MOUSE_HH
