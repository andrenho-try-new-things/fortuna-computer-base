#include "rtc.hh"

#include <pico/aon_timer.h>
#include <ctime>

// NOTE - the `aon` functions use `mktime` which increases the binary in about 41 kB.
// This can be fixed, if necessary, by reimplementing `pico_mktime`.
// https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#group_pico_aon_timer_1rp2350_caveats

namespace rtc {

void init()
{
    aon_timer_start_with_timeofday();
}

DateTime get()
{
    tm t {};
    if (aon_timer_get_time_calendar(&t)) {
        return {
            .year = (uint16_t) (t.tm_year + 1900),
            .month = (uint8_t) (t.tm_mon + 1),
            .day = (uint8_t) t.tm_mday,
            .weekday = (uint8_t) t.tm_wday,
            .hours = (uint8_t) t.tm_hour,
            .minutes = (uint8_t) t.tm_min,
            .seconds = (uint8_t) t.tm_sec,
        };
    }
    return {};
}

void set(DateTime const& datetime)
{

}

}
