#ifndef USER_HH
#define USER_HH

#include <cstdint>

namespace user {

void init();
void init_interupts();

void    set_led(bool value);
uint8_t get_dipswitch();

struct Event {
    enum Button : uint8_t { PushButton, Switch0, Switch1, Unknown };
    Button button;
    bool   new_value;
};

}

#endif //USER_HH
