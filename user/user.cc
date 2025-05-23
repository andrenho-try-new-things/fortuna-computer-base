#include "user.hh"

#include <hardware/gpio.h>

#include "../fortuna/fortuna.hh"

namespace user {

static const uint8_t LED_PIN = 14;
static const uint8_t DIP0_PIN = 27;
static const uint8_t DIP1_PIN = 28;
static const uint8_t USER_BUTTON_PIN = 15;

void init()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(DIP0_PIN);
    gpio_init(DIP1_PIN);
    gpio_pull_up(DIP0_PIN);
    gpio_pull_up(DIP1_PIN);
    gpio_set_dir(DIP0_PIN, GPIO_IN);
    gpio_set_dir(DIP1_PIN, GPIO_IN);

    gpio_init(USER_BUTTON_PIN);
    gpio_pull_up(USER_BUTTON_PIN);
    gpio_set_dir(USER_BUTTON_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(USER_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, [](uint, uint32_t) {
        fortuna::add_event(fortuna::Event {
            .type = fortuna::Event::Type::UserButton,
            .has_data = true,
        });
    });
}

void set_led(bool value)
{
    gpio_put(LED_PIN, value);
}

uint8_t get_dipswitch()
{
    uint8_t v = gpio_get(DIP1_PIN) << 1 | gpio_get(DIP0_PIN);
    return (~v) & 0b11;
}

}
