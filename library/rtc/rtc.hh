#ifndef CLOCK_HH
#define CLOCK_HH

#include <cstdint>

namespace rtc {

struct DateTime {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  weekday;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
};

void     init();
DateTime get();
void     set(DateTime const& datetime);

}

#endif //CLOCK_HH
