/**
 * Hunter Adams (vha3@cornell.edu)
 * 
 * HARDWARE CONNECTIONS
 *  - GPIO 16 ---> VGA Hsync
 *  - GPIO 17 ---> VGA Vsync
 *  - GPIO 18 ---> 330 ohm resistor ---> VGA Red
 *  - GPIO 19 ---> 330 ohm resistor ---> VGA Green
 *  - GPIO 20 ---> 330 ohm resistor ---> VGA Blue
 *  - RP2040 GND ---> VGA GND
 *
 * RESOURCES USED
 *  - PIO state machines 0, 1, and 2 on PIO instance 0
 *  - DMA channels 0, 1, 2, and 3
 *  - 153.6 kBytes of RAM (for pixel color data)
 *
 * Protothreads v1.1.1
 * Threads:
 * core 0:
 * Graphics demo
 * blink LED25 
 * core 1:
 * Toggle gpio 4 
 * Serial i/o 
 */
// ==========================================
// === VGA graphics library
// ==========================================
#include "vga16_graphics.h"
#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
// // Our assembled programs:
// // Each gets the name <pio_filename.pio.h>
// #include "hsync.pio.h"
// #include "vsync.pio.h"
// #include "rgb.pio.h"

// ==========================================
// === protothreads globals
// ==========================================
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "pico/multicore.h"
#include "string.h"
// protothreads header
#include "pt_cornell_rp2040_v1_1_1.h"

// ==========================================

// Some globals for storing timer information
volatile unsigned int time_accum = 0;
unsigned int time_accum_old = 0 ;
char timetext[40];

// Timer interrupt
bool repeating_timer_callback(struct repeating_timer *t) {

    time_accum += 1 ;
    return true;
}

// ==================================================
// === graphics demo -- RUNNING on core 0
// ==================================================
static PT_THREAD (protothread_graphics(struct pt *pt)) {
    PT_BEGIN(pt);
    // the protothreads interval timer
    PT_INTERVAL_INIT() ;

    // circle radii
    static short circle_x = 0 ;

    // color chooser
    static char color_index = 0 ;

    // position of the disc primitive
    static short disc_x = 0 ;
    // position of the box primitive
    static short box_x = 0 ;
    // position of vertical line primitive
    static short Vline_x = 350;
    // position of horizontal line primitive
    static short Hline_y = 250;

    // Draw some filled rectangles
    fillRect(64, 0, 176, 50, BLUE); // blue box
    fillRect(250, 0, 176, 50, DARK_ORANGE); // red box
    fillRect(435, 0, 176, 50, GREEN); // green box

    // Write some text
    setTextColor(WHITE) ;
    setCursor(65, 0) ;
    setTextSize(1) ;
    writeString("Raspberry Pi Pico") ;
    setCursor(65, 10) ;
    writeString("Graphics primitives demo") ;
    setCursor(65, 20) ;
    writeString("Hunter Adams") ;
    setCursor(65, 30) ;
    writeString("vha3@cornell.edu") ;
    setCursor(445, 10) ;
    setTextColor(BLACK) ;
    setTextSize(1) ;
    writeString("Protothreads rp2040 v1.1.1") ;
    setTextColor(WHITE) ;

    // Setup a 1Hz timer
    static struct repeating_timer timer;
    add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);

     char video_buffer[32];
    setTextColor2(WHITE, BLACK) ;
    setTextSize(1) ;
    for (int i=0; i<4; i++) {
      for (int j=0; j<4; j++){
        fillRect(i*70+20, 150+j*70, 60, 60, i+4*j);  
        setCursor(i*70+20, 150+j*70) ; 
        sprintf(video_buffer, "%2d", i+4*j);
        writeString(video_buffer) ;
      }
    }
    // first row of colors
    setCursor(0*70+20, 200+0*70) ; 
    writeString("BLACK") ;
    setCursor(1*70+20, 200+0*70) ; 
    writeString("DARK_GREEN") ;
    setCursor(2*70+20, 200+0*70) ; 
    writeString("MED_GREEN") ;
    setCursor(3*70+20, 200+0*70) ; 
    writeString("GREEN") ;
    // second row of colors
    setCursor(0*70+20, 200+1*70) ; 
    writeString("DARK_BLUE") ;
    setCursor(1*70+20, 200+1*70) ; 
    writeString("BLUE") ;
    setCursor(2*70+20, 200+1*70) ; 
    writeString("LIGHT_BLUE") ;
    setCursor(3*70+20, 200+1*70) ; 
    writeString("CYAN") ;
    // thrid row of colors
    setCursor(0*70+20, 200+2*70) ; 
    writeString("RED") ;
    setCursor(1*70+20, 200+2*70) ; 
    writeString("DARK_ORANGE") ;
    setCursor(2*70+20, 200+2*70) ; 
    writeString("ORANGE") ;
    setCursor(3*70+20, 200+2*70) ; 
    writeString("YELLOW") ;
    // fourth row of colors
    setCursor(0*70+20, 200+3*70) ; 
    writeString("MAGENTA") ;
    setCursor(1*70+20, 200+3*70) ; 
    writeString("PINK") ;
    setCursor(2*70+20, 200+3*70) ; 
    writeString("LIGHT_PINK") ;
    setCursor(3*70+20, 200+3*70) ; 
    writeString("WHITE") ;

    while(true) {

        // Modify the color chooser
        if (color_index ++ == 15) color_index = 0 ;

        // A row of filled circles
        fillCircle(disc_x, 100, 20, color_index);
        disc_x += 35 ;
        if (disc_x > 640) disc_x = 0;

       
        // A brief nap
        PT_YIELD_usec(100000) ;
   }
   PT_END(pt);
} // graphics thread

// ==================================================
// === toggle25 thread on core 0
// ==================================================
// the on-board LED blinks
static PT_THREAD (protothread_toggle25(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;
    
     // set up LED p25 to blink
     gpio_init(25) ;	
     gpio_set_dir(25, GPIO_OUT) ;
     gpio_put(25, true);
     // data structure for interval timer
     PT_INTERVAL_INIT() ;

      while(1) {
        // yield time 0.1 second
        //PT_YIELD_usec(100000) ;
        PT_YIELD_INTERVAL(100000) ;

        // toggle the LED on PICO
        LED_state = LED_state? false : true ;
        gpio_put(25, LED_state);
        //
        // NEVER exit while
      } // END WHILE(1)
  PT_END(pt);
} // blink thread


// ==================================================
// === toggle gpio 4 thread -- RUNNING on core 1
// ==================================================
// toggle gpio 4 
static PT_THREAD (protothread_toggle_gpio4(struct pt *pt))
{
    PT_BEGIN(pt);
    static bool LED_state = false ;
    //
     // set up LED gpio 4 to blink
     gpio_init(4) ;	
     gpio_set_dir(4, GPIO_OUT) ;
     gpio_put(4, true);
     // data structure for interval timer
     PT_INTERVAL_INIT() ;

      while(1) {
        //
        PT_YIELD_INTERVAL(20) ;
        // toggle gpio 4
        LED_state = !LED_state ;
        gpio_put(4, LED_state);
        //
        // NEVER exit while
      } // END WHILE(1)
  PT_END(pt);
} // blink thread

// ==================================================
// === user's serial input thread on core 1
// ==================================================
// serial_read an serial_write do not block any thread
// except this one
static PT_THREAD (protothread_serial(struct pt *pt))
{
    PT_BEGIN(pt);
      static int test_in1, test_in2, sum ;
      //
      while(1) {
        // print prompt
        sprintf(pt_serial_out_buffer, "input two numbers: ");
        // spawn a thread to do the non-blocking write
        serial_write ;

        // spawn a thread to do the non-blocking serial read
         serial_read ;
        // convert input string to number
        sscanf(pt_serial_in_buffer,"%d %d", &test_in1, &test_in2) ;

        // add and convert back to sring
        sprintf(pt_serial_out_buffer,"sum = %d\r\n", test_in1 + test_in2);
        // spawn a thread to do the non-blocking serial write
         serial_write ;

        // NEVER exit while
      } // END WHILE(1)
  PT_END(pt);
} // serial thread

// ========================================
// === core 1 main -- started in main below
// ========================================
void core1_main(){ 
  //
  //  === add threads  ====================
  // for core 1
  pt_add_thread(protothread_toggle_gpio4) ;
  pt_add_thread(protothread_serial) ;
  //
  // === initalize the scheduler ==========
  pt_schedule_start ;
  // NEVER exits
  // ======================================
}

// ========================================
// === core 0 main
// ========================================
int main(){
  // set the clock
  //set_sys_clock_khz(250000, true); // 171us
  // start the serial i/o
  stdio_init_all() ;
  // announce the threader version on system reset
  printf("\n\rProtothreads RP2040 v1.1 two-core\n\r");

  // Initialize the VGA screen
  initVGA() ;
     
  // start core 1 threads
  multicore_reset_core1();
  multicore_launch_core1(&core1_main);

  // === config threads ========================
  // for core 0
  pt_add_thread(protothread_graphics);
  pt_add_thread(protothread_toggle25);
  //
  // === initalize the scheduler ===============
  pt_schedule_start ;
  // NEVER exits
  // ===========================================
} // end main