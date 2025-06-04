#include "mouse.hh"

static uint8_t device_id;
static uint8_t instance;

void usb::mouse::init(uint8_t device_id_, uint8_t instance_)
{
    device_id = device_id_;
    instance = instance_;
    tuh_hid_set_protocol(device_id, instance, HID_PROTOCOL_REPORT);
}

void usb::mouse::process_report(hid_mouse_report_t const* report)
{
//#if 0   // enable this to print the mouse state
    printf("buttons %02X\n", report->buttons);
    printf("pan     %02X\n", report->pan);
    printf("wheel   %02X\n", report->wheel);
    printf("x, y    %d, %d\n", report->x, report->y);
    printf("---------------------\n");
//#endif
}
