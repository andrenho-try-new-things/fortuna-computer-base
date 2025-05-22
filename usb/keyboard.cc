#include "keyboard.hh"

static uint8_t const keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

namespace usb::keyboard {

static void(*subscription)(Event const& event) = nullptr;
static uint8_t device_id;
static uint8_t instance;
static hid_keyboard_report_t prev = {};

static hid_keyboard_led_bm_t leds = KEYBOARD_LED_NUMLOCK;

void init(uint8_t device_id_, uint8_t instance_)
{
    device_id = device_id_;
    instance = instance_;
    tuh_hid_set_report(device_id, instance, 0, HID_REPORT_TYPE_OUTPUT, &leds, sizeof(leds));
}

void subscribe(void(* event_cb)(Event const& event))
{
    subscription = event_cb;
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
#if 1   // enable this to print the character state
    for(uint8_t i=0; i<6; i++)
        printf("[%02X]", report->keycode[i]);
    printf("  mod: %02X\n", report->modifier);
#endif

    auto fire_event = [](hid_keyboard_report_t const* report, uint8_t i, bool pressed) {
        subscription({
            .hid_key = report->keycode[i],
            .chr = keychar(report, i),
            .ctrl = (bool) (report->modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL)),
            .alt = (bool) (report->modifier & (KEYBOARD_MODIFIER_LEFTALT | KEYBOARD_MODIFIER_RIGHTALT)),
            .shift = (bool) (report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT)),
            .pressed = pressed,
        });
    };

    if (subscription) {

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
    }

    prev = *report;
}


/*
//--------------------------------------------------------------------+
// Keyboard
//--------------------------------------------------------------------+

// look up new key in previous keys
static bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t keycode)
{
    for (uint8_t i = 0; i < 6; i++)
        if (report->keycode[i] == keycode)
            return true;

    return false;
}

static void process_kbd_report(hid_keyboard_report_t const *report)
{
    static hid_keyboard_report_t prev_report = { 0, 0, {0} }; // previous report to check key released

    //------------- example code ignore control (non-printable) key affects -------------//
    for(uint8_t i=0; i<6; i++)
    {
        printf("[%02X]", report->keycode[i]);

        if ( report->keycode[i] )
        {
            if ( find_key_in_report(&prev_report, report->keycode[i]) )
            {
                // exist in previous report means the current key is holding
            }else
            {
                // not existed in previous report means the current key is pressed
                bool const is_shift = report->modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT);
                uint8_t ch = keycode2ascii[report->keycode[i]][is_shift ? 1 : 0];
                putchar(ch);
                putchar('\n');
                if ( ch == '\r' ) putchar('\n'); // added new line for enter key

                // fflush(stdout); // flush right away, else nanolib will wait for newline
            }
        }
        // TODO example skips key released
    }

    printf("\n");

    prev_report = *report;
}
*/

}
