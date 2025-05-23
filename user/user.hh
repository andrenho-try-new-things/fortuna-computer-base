#ifndef USER_HH
#define USER_HH

#include <cstdint>

namespace user {

void init();

void    set_led(bool value);
uint8_t get_dipswitch();

}

#endif //USER_HH
