#include "rtc.hh"

#include <cstdio>
#include <ctime>

#include "pico/aon_timer.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// NOTE - the `aon` functions use `mktime` which increases the binary in about 41 kB.
// This can be fixed, if necessary, by reimplementing `pico_mktime`.
// https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#group_pico_aon_timer_1rp2350_caveats

namespace rtc {

static constexpr uint8_t SDA_PIN = 8;
static constexpr uint8_t SCL_PIN = 9;
static constexpr uint8_t DS1307_ADDR = 0x68;

static i2c_inst_t* i2c = i2c0;

static void read_ds1307_and_upload_to_pico()
{
    uint8_t data[7] = {};

    // read device id
    uint8_t addr_read = 0x0;
    i2c_write_blocking(i2c, DS1307_ADDR, &addr_read, 1, false);
    i2c_read_blocking(i2c, DS1307_ADDR, data, 7, true);

    printf("Datetime received from DS1307: %02X %02X %02X %02X %02X %02X %02X\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6]);

    // set pico RTC
    tm t = {
        .tm_sec = (data[0] & 0xf) + ((data[0] & 0x7f) >> 4) * 10,
        .tm_min = (data[1] & 0xf) + (data[1] >> 4) * 10,
        .tm_mday = (data[4] & 0xf) + (data[4] >> 4) * 10,
        .tm_mon = (data[5] & 0xf) + (data[5] >> 4) * 10 - 1,
        .tm_year = (data[6] & 0xf) + (data[6] >> 4) * 10 + 100,
    };
    if (((data[2] >> 6) & 1) == 1) { // 12 hr
        t.tm_hour = data[2] & 0xf;
        if (((data[2] >> 5) & 1) == 1) // PM
            t.tm_hour += 12;
    } else {
        t.tm_hour = (data[2] & 0xf) + (data[2] >> 4) * 10;
    }

    aon_timer_start_calendar(&t);
}

void init()
{
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    read_ds1307_and_upload_to_pico();
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

void set(DateTime const& d)
{
    uint8_t data[] = {
        0,
        (uint8_t) ((d.seconds % 10) | ((d.seconds / 10) << 4)),
        (uint8_t) ((d.minutes % 10) | ((d.minutes/ 10) << 4)),
        (uint8_t) ((d.hours % 10) | (((d.hours / 10) & 0b11) << 4)),
        0,
        (uint8_t) ((d.day % 10) | ((d.day / 10) << 4)),
        (uint8_t) ((d.month % 10) | ((d.month / 10) << 4)),
        (uint8_t) ((d.year % 10) | (((d.year % 100) / 10) << 4)),
    };

    // read device id
    i2c_write_blocking(i2c, DS1307_ADDR, data, 8, true);
    printf("Datetime sent to DS1307: %02X %02X %02X %02X %02X %02X %02X\n", data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

    read_ds1307_and_upload_to_pico();
}

}
