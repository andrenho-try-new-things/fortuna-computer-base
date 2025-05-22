#include "keyboard.hh"

#define MAX_EVENTS 16

namespace usb::keyboard {

static uint8_t const keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

static uint8_t device_id;
static uint8_t instance;
static hid_keyboard_report_t prev = {};

static hid_keyboard_led_bm_t leds = KEYBOARD_LED_NUMLOCK;

static Event event_queue[MAX_EVENTS] = {0};

void init(uint8_t device_id_, uint8_t instance_)
{
    device_id = device_id_;
    instance = instance_;
    tuh_hid_set_report(device_id, instance, 0, HID_REPORT_TYPE_OUTPUT, &leds, sizeof(leds));
}

bool next_event(Event* event)
{
    for (int8_t i = MAX_EVENTS - 1; i >= 0; --i) {
        if (event_queue[i].has_data) {
            *event = event_queue[i];
            event_queue[i].has_data = false;
            return true;
        }
    }

    return false;
}

static void add_event(Event const& event)
{
    for (uint8_t i = 0; i < MAX_EVENTS; ++i) {
        if (!event_queue[i].has_data) {
            event_queue[i] = event;
            return;
        }
    }

    // no space
    memmove(event_queue, &event_queue[1], sizeof(Event) * (MAX_EVENTS - 1));
    event_queue[MAX_EVENTS - 1] = event;
}

static void process_special_key(uint8_t keycode)
{
    if (keycode == HID_KEY_CAPS_LOCK || keycode == HID_KEY_NUM_LOCK) {
        if (keycode == HID_KEY_CAPS_LOCK) {
            if (leds & KEYBOARD_LED_CAPSLOCK)
                leds = (hid_keyboard_led_bm_t) (leds & ~KEYBOARD_LED_CAPSLOCK);
            else
                leds = (hid_keyboard_led_bm_t) (leds | KEYBOARD_LED_CAPSLOCK);
        } else if (keycode == HID_KEY_NUM_LOCK) {
            if (leds & KEYBOARD_LED_NUMLOCK)
                leds = (hid_keyboard_led_bm_t) (leds & ~KEYBOARD_LED_NUMLOCK);
            else
                leds = (hid_keyboard_led_bm_t) (leds | KEYBOARD_LED_NUMLOCK);
        }
        tuh_hid_set_report(device_id, instance, 0, HID_REPORT_TYPE_OUTPUT, &leds, sizeof(leds));
    }
}

static uint8_t keychar(hid_keyboard_report_t const* report, uint8_t i)
{
    if (leds & KEYBOARD_LED_CAPSLOCK) {
        if (report->keycode[i] >= HID_KEY_A && report->keycode[i] <= HID_KEY_Z)
            return keycode2ascii[report->keycode[i]][1];
    }

    bool is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
    return keycode2ascii[report->keycode[i]][is_shift ? 1 : 0];
}

void process_report(hid_keyboard_report_t const* report)
{
#if 0   // enable this to print the character state
    for(uint8_t i=0; i<6; i++)
        printf("[%02X]", report->keycode[i]);
    printf("  mod: %02X\n", report->modifier);
#endif

    auto fire_event = [](hid_keyboard_report_t const* report, uint8_t i, bool pressed) {
        add_event({
            .hid_key = report->keycode[i],
            .chr = keychar(report, i),
            .ctrl = (bool) (report->modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL)),
            .alt = (bool) (report->modifier & (KEYBOARD_MODIFIER_LEFTALT | KEYBOARD_MODIFIER_RIGHTALT)),
            .shift = (bool) (report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT)),
            .pressed = pressed,
            .has_data = true,
        });
    };

    // look for new keys
    for (uint8_t i = 0; i < 6; ++i) {
        if (report->keycode[i] != 0) {
            bool found = false;
            for (uint8_t j = 0; j < 6; ++j)
                if (report->keycode[i] == prev.keycode[j])
                    found = true;

            if (!found) {
                process_special_key(report->keycode[i]);
                fire_event(report, i, true);
            }
        }
    }

    // look for keys released
    for (uint8_t i = 0; i < 6; ++i) {
        if (prev.keycode[i] != 0) {
            bool found = false;
            for (uint8_t j = 0; j < 6; ++j)
                if (prev.keycode[i] == report->keycode[j])
                    found = true;

            if (!found)
                fire_event(&prev, i, false);
        }
    }

    prev = *report;
}

}
