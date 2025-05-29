#include "user.hh"

#include <hardware/gpio.h>

#include "../fortuna/fortuna.hh"

namespace user {

static constexpr uint8_t LED_PIN = 14;
static constexpr uint8_t DIP0_PIN = 27;
static constexpr uint8_t DIP1_PIN = 28;
static constexpr uint8_t PUSHBUTTON_PIN = 15;

static constexpr uint16_t USER_BUTTON_DECOMPRESS = 200;
static absolute_time_t last_button_press = 0;

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

    gpio_init(PUSHBUTTON_PIN);
    gpio_pull_up(PUSHBUTTON_PIN);
    gpio_set_dir(PUSHBUTTON_PIN, GPIO_IN);
}

void init_interupts()
{
    gpio_set_irq_callback([](uint gpio, uint32_t) {

        Event::Button button = Event::Unknown;
        switch (gpio) {
            case PUSHBUTTON_PIN: button = Event::PushButton; break;
            case DIP0_PIN: button = Event::Switch0; break;
            case DIP1_PIN: button = Event::Switch1; break;
        }

        if (gpio == PUSHBUTTON_PIN && to_ms_since_boot(last_button_press) + USER_BUTTON_DECOMPRESS > to_ms_since_boot(get_absolute_time()))
            return;
        last_button_press = get_absolute_time();

        fortuna::add_event(fortuna::Event {
            .type = fortuna::Event::Type::UserPanel,
            .user = { .button = button, .new_value = !gpio_get(gpio) },
        });
    });
    gpio_set_irq_enabled(PUSHBUTTON_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(DIP0_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(DIP1_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
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
