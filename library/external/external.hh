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
void step();

}

#endif //EXTERNAL_HH
