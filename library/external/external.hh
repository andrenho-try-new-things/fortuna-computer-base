#ifndef EXTERNAL_HH
#define EXTERNAL_HH

#include <cstddef>
#include <cstdint>

namespace external {

constexpr size_t BUFFER_RECV_SZ = 16;
struct Buffer {
    uint8_t sz;
    uint8_t data[BUFFER_RECV_SZ];
};

void init();
void add_response(uint8_t* data, size_t sz);
void change_default_response(uint8_t value);
void send_request_to_event_queue(bool value);

}

#endif //EXTERNAL_HH
