#ifndef VGA_H_
#define VGA_H_

#include "fb.hh"
#include "text.hh"

namespace vga {

extern unsigned char vga_data_array[];

void init();

};

#endif