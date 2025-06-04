#include "mouse.hh"

#include <fortuna.hh>

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
    fortuna::add_event(fortuna::Event {
        .type = fortuna::Event::Type::Mouse,
        .mouse = {
            .x = report->x,
            .y = report->y,
            .wheel = report->wheel,
            .buttons = report->buttons,
        }
    });

    vga::update_mouse_position(report->x, report->y);
}
