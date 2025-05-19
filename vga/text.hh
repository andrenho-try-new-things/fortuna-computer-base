#ifndef TEXT_HH
#define TEXT_HH

#include <cstdint>

namespace vga::text {

void init();
void print(uint8_t c);
void print(const char* text);

}

#endif //TEXT_HH
