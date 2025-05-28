#include "audio.hh"

#include <cstdint>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

namespace audio {

static constexpr uint8_t AUDIO_PIN = 7;


static void set_frequency(uint8_t slice_no, float frequency)
{
    // PWM frequency = clock_freq / (divider * (wrap + 1))
    float clock_freq = clock_get_hz(clk_sys);

    // Target: 50% duty cycle, reasonable wrap value
    uint32_t divider = (uint32_t)(clock_freq / (frequency * 65536.0f));
    if (divider < 1) divider = 1;
    if (divider > 255) divider = 255;

    uint32_t wrap = (uint32_t)(clock_freq / (frequency * divider)) - 1;

    // Set the divider and wrap
    pwm_set_clkdiv(slice_no, (float)divider);
    pwm_set_wrap(slice_no, wrap);

    // Set 50% duty cycle
    pwm_set_chan_level(slice_no, PWM_CHAN_B, wrap / 2);
}

void init()
{
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    uint slice_no = pwm_gpio_to_slice_num(AUDIO_PIN);
    uint channel = pwm_gpio_to_channel(AUDIO_PIN);

    /*
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_no, &config, false);

    set_frequency(slice_no, 440);
    pwm_set_enabled(slice_no, true);
    */

    pwm_set_wrap(slice_no, 50000);
    pwm_set_chan_level(slice_no, channel, 50000 / 2);
    pwm_set_enabled(slice_no, true);
}

}
