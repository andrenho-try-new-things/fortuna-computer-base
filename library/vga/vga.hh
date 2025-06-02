#ifndef VGA_H_
#define VGA_H_

#include "fb.hh"
#include "text.hh"

namespace vga {

extern uint8_t* vga_data_array;

extern uint16_t screen_width;
extern uint16_t screen_height;

enum class Mode { V_640x480, V_640x240, V_320x240, V_SPRITES };

void init();
void set_mode(Mode mode);
void step();

};

#endif