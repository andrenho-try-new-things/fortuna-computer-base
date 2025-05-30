#ifndef VGA_H_
#define VGA_H_

#include "fb.hh"
#include "text.hh"

namespace vga {

extern unsigned char vga_data_array[];

enum class Mode { V_640x480, V_320x240 };

void init();
void set_mode(Mode mode);

};

#endif