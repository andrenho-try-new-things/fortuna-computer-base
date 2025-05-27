#ifndef EXTERNAL_HH
#define EXTERNAL_HH

#include <cstddef>
#include <cstdint>

namespace external {

constexpr size_t BUFFER_RECV_SZ = 16;

void init(uint32_t speed=1000*1000);
void step();

}

#endif //EXTERNAL_HH
