#include "audio.hh"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <utility>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/util/queue.h>

namespace audio {

static constexpr uint8_t AUDIO_PIN = 7;
static uint slice_no;
static uint channel;

static Sound* music = nullptr;
static size_t music_sz = 0;
static size_t current_note = 0;
static bool   music_nonstop = false;
static bool   music_playing = false;

static alarm_pool_t* alarm_pool;

static const std::pair<uint16_t, uint16_t> pwm_top(float desired_frequency, uint16_t divider=128)
{
    uint64_t top = SYS_CLK_HZ / desired_frequency / divider;
    if (top > 65535)
        return pwm_top(desired_frequency, divider / 2);
    return { top, divider };
}

void init()
{
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    slice_no = pwm_gpio_to_slice_num(AUDIO_PIN);
    channel = pwm_gpio_to_channel(AUDIO_PIN);
    alarm_pool = alarm_pool_create_with_unused_hardware_alarm(2);   // allows running alarm on core 1
}

void set_music(Sound* sounds, size_t sz)
{
    music = (Sound *) realloc(music, sz * sizeof(Sound));
    memcpy(music, sounds, sz * sizeof(Sound));
    music_sz = sz;
}

static void play_sound(Sound const& sound)
{
    if (sound.note == Pause) {
        pwm_set_enabled(slice_no, false);
    } else {
        auto [top, divider] = pwm_top((float) sound.note / 1000.f);
        pwm_set_clkdiv(slice_no, divider);
        pwm_set_wrap(slice_no, top);
        pwm_set_chan_level(slice_no, channel, top / 2);
    }
}

static int64_t play_next_note(alarm_id_t id = -1, void *user_data = nullptr)
{
    if (!music_playing) {
        pwm_set_enabled(slice_no, false);
        return 0;
    }

    if (current_note >= music_sz) {
        if (music_nonstop) {
            current_note = 0;
        } else {
            pwm_set_enabled(slice_no, false);
            return 0;
        }
    }

    play_sound(music[current_note]);

    ++current_note;
    if (id != -1)
        alarm_pool_cancel_alarm(alarm_pool, id);
    alarm_pool_add_alarm_in_ms(alarm_pool, music[current_note].time, play_next_note, nullptr, true);
    return 0;
}

void play_single_note(Sound const& sound)
{
    if (!music_playing) {
        play_sound(sound);
        alarm_pool_add_alarm_in_ms(alarm_pool, sound.time, [](alarm_id_t, void*) -> int64_t {
            pwm_set_enabled(slice_no, false);
            return 0;
        }, nullptr, true);
    }
}

void play_music(bool nonstop)
{
    music_nonstop = nonstop;
    current_note = 0;
    music_playing = true;
    play_next_note();
}

void stop_music()
{
    music_playing = false;
    current_note = 0;
}

}
