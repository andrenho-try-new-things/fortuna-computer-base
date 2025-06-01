// original code by Hunter Adams (vha3@cornell.edu), modified by Andre Wagner

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "vga.hh"

#include "hsync.pio.h"
#include "vsync.pio.h"
#include "rgb640.pio.h"
#include "rgb320.pio.h"


enum vga_pins {HSYNC=16, VSYNC, LO_GRN, HI_GRN, BLUE_PIN, RED_PIN} ;

// VGA timing constants
#define H_ACTIVE   655    // (active + frontporch - 1) - one cycle delay for mov
#define V_ACTIVE   479    // (active - 1)
#define RGB_ACTIVE 319    // (horizontal active)/2 - 1
// #define RGB_ACTIVE 639 // change to this if 1 pixel/byte

// Length of the pixel array, and number of DMA transfers
#define TXCOUNT 153600 // Total pixels/2 (since we have 2 pixels per byte)

// Pixel color array that is DMA's to the PIO machines and
// a pointer to the ADDRESS of this color array.
// Note that this array is automatically initialized to all 0's (black)
// Bit masks for drawPixel routine
#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

namespace vga {

// VGA framebuffer
unsigned char vga_data_array[TXCOUNT];
static void* address_pointer = &vga_data_array[0] ;

static bool first_setup = true;
static int rgb_chan_0, rgb_chan_1;
static uint rgb_offset, vsync_offset, hsync_offset;

// state machines
constexpr uint HSYNC_SM = 0;
constexpr uint VSYNC_SM = 1;
constexpr uint RGB_SM = 2;

int current_scanline = 0;
uint32_t current_frame = 0;

Mode current_mode = Mode::V_640x480;
uint16_t screen_width = 640;
uint16_t screen_height = 480;

static volatile bool new_resolution = false;

dma_channel_config c0, c1;

static void dma_handler()
{
    // Clear the interrupt request for DMA control channel
    dma_hw->ints0 = (1u << rgb_chan_0);

    // increment scanline (1..)
    current_scanline++;                  // new current scanline
    if (current_scanline >= 480) {       // last scanline?
        current_scanline = 0;            // restart scanline
        current_frame++;                 // increment frame counter

        if (new_resolution) {
            // pio_sm_exec(pio0, RGB_SM, pio_encode_jmp(0));
            new_resolution = false;
        }
    }

    if (current_mode == Mode::V_640x480)
        address_pointer = &vga_data_array[(screen_width / 2) * current_scanline];
    else
        address_pointer = &vga_data_array[(screen_width / 2) * (current_scanline / 2)];
}


void init_dma()
{
    // Channel Zero (sends color data to PIO VGA machine)
    c0 = dma_channel_get_default_config(rgb_chan_0);  // default configs
    channel_config_set_transfer_data_size(&c0, DMA_SIZE_8);              // 8-bit txfers
    channel_config_set_read_increment(&c0, true);                        // yes read incrementing
    channel_config_set_write_increment(&c0, false);                      // no write incrementing
    channel_config_set_dreq(&c0, DREQ_PIO0_TX2) ;                        // DREQ_PIO0_TX2 pacing (FIFO)
    channel_config_set_chain_to(&c0, rgb_chan_1);                        // chain to other channel

    dma_channel_configure(
        rgb_chan_0,                 // Channel to be configured
        &c0,                        // The configuration we just created
        &pio0->txf[RGB_SM],         // write address (RGB PIO TX FIFO)
        &vga_data_array,            // The initial read address (pixel color array)
        screen_width / 2,           // Number of transfers; in this case each is 1 byte.
        false                       // Don't start immediately.
    );

    // Channel One (reconfigures the first channel)
    c1 = dma_channel_get_default_config(rgb_chan_1);   // default configs
    channel_config_set_transfer_data_size(&c1, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c1, false);                        // no read incrementing
    channel_config_set_write_increment(&c1, false);                       // no write incrementing
    channel_config_set_chain_to(&c1, rgb_chan_0);                         // chain to other channel

    dma_channel_configure(
        rgb_chan_1,                         // Channel to be configured
        &c1,                                // The configuration we just created
        &dma_hw->ch[rgb_chan_0].read_addr,  // Write address (channel 0 read address)
        &address_pointer,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

    // enable DMA
    dma_channel_set_irq0_enabled(rgb_chan_0, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    irq_set_priority(DMA_IRQ_0, 0);

    // start DMA channel
    dma_start_channel_mask((1u << rgb_chan_0)) ;
}


void set_mode(Mode mode)
{
    /*
    dma_channel_wait_for_finish_blocking(rgb_chan_0);
    dma_channel_abort(rgb_chan_0);
    dma_channel_cleanup(rgb_chan_0);

    dma_channel_wait_for_finish_blocking(rgb_chan_1);
    dma_channel_abort(rgb_chan_1);
    dma_channel_cleanup(rgb_chan_1);
    */

    dma_channel_wait_for_finish_blocking(rgb_chan_1);
    dma_channel_wait_for_finish_blocking(rgb_chan_0);

    pio_sm_set_enabled(pio0, RGB_SM, false);
    pio_sm_set_enabled(pio0, VSYNC_SM, false);
    pio_sm_set_enabled(pio0, HSYNC_SM, false);

    switch (mode) {
        case Mode::V_640x480:
            screen_width = 640;
            screen_height = 480;
            pio_remove_program(pio0, &rgb320_program, rgb_offset);
            rgb_offset = pio_add_program(pio0, &rgb640_program);
            break;
        case Mode::V_320x240:
            screen_width = 320;
            screen_height = 240;
            pio_remove_program(pio0, &rgb640_program, rgb_offset);
            rgb_offset = pio_add_program(pio0, &rgb320_program);
            break;
    }
    current_mode = mode;

    // pio_enable_sm_mask_in_sync(pio0, 0);

    current_scanline = 0;

    pio_sm_clear_fifos(pio0, RGB_SM);
    pio_sm_put_blocking(pio0, RGB_SM, (screen_width / 2) - 1);
    pio_sm_exec(pio0, RGB_SM, pio_encode_jmp(rgb_offset));

    pio_sm_clear_fifos(pio0, HSYNC_SM);
    pio_sm_put_blocking(pio0, HSYNC_SM, H_ACTIVE);
    pio_sm_exec(pio0, HSYNC_SM, pio_encode_jmp(hsync_offset));

    pio_sm_clear_fifos(pio0, VSYNC_SM);
    pio_sm_put_blocking(pio0, VSYNC_SM, V_ACTIVE);
    pio_sm_exec(pio0, VSYNC_SM, pio_encode_jmp(vsync_offset));

    dma_channel_configure(
        rgb_chan_0,                 // Channel to be configured
        &c0,                        // The configuration we just created
        &pio0->txf[RGB_SM],         // write address (RGB PIO TX FIFO)
        &vga_data_array,            // The initial read address (pixel color array)
        screen_width / 2,           // Number of transfers; in this case each is 1 byte.
        false                       // Don't start immediately.
    );

    /*
    dma_channel_set_read_addr(rgb_chan_1, &address_pointer, false);
    dma_channel_configure(
        rgb_chan_1,                         // Channel to be configured
        &c1,                                // The configuration we just created
        &dma_hw->ch[rgb_chan_0].read_addr,  // Write address (channel 0 read address)
        &address_pointer,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );
    */

    pio_enable_sm_mask_in_sync(pio0, ((1u << HSYNC_SM) | (1u << VSYNC_SM) | (1u << RGB_SM)));

    new_resolution = true;
}

void init_640()
{
    screen_width = 640;
    screen_height = 480;
    current_mode = Mode::V_640x480;

    // load programs
    hsync_offset = pio_add_program(pio0, &hsync_program);
    vsync_offset = pio_add_program(pio0, &vsync_program);
    rgb_offset = pio_add_program(pio0, &rgb640_program);
    printf("hsync_offset = %d\n", hsync_offset);
    printf("vsync_offset = %d\n", vsync_offset);
    printf("rgb_offset = %d\n", rgb_offset);

    // Manually select a few state machines from pio instance pio0.
    // initialize programs (C function in pio files)
    hsync_program_init(pio0, HSYNC_SM, hsync_offset, HSYNC);
    vsync_program_init(pio0, VSYNC_SM, vsync_offset, VSYNC);
    rgb640_program_init(pio0, RGB_SM, rgb_offset, LO_GRN);

    // Initialize PIO state machine counters.
    pio_sm_put_blocking(pio0, HSYNC_SM, H_ACTIVE);
    pio_sm_put_blocking(pio0, VSYNC_SM, V_ACTIVE);
    pio_sm_put_blocking(pio0, RGB_SM, (screen_width / 2) - 1);

    // Start the two pio machine IN SYNC

    // DMA channels - 0 sends color data, 1 reconfigures and restarts 0
    rgb_chan_0 = dma_claim_unused_channel(true);
    printf("rgb_chan_0 = %d\n", rgb_chan_0);
    rgb_chan_1 = dma_claim_unused_channel(true);
    printf("rgb_chan_1 = %d\n", rgb_chan_1);

    init_dma();

    pio_enable_sm_mask_in_sync(pio0, ((1u << HSYNC_SM) | (1u << VSYNC_SM) | (1u << RGB_SM)));

    text::init();

    printf("VGA initialized.\n");
}

void init_320()
{
    screen_width = 320;
    screen_height = 240;
    current_mode = Mode::V_320x240;

    // load programs
    hsync_offset = pio_add_program(pio0, &hsync_program);
    vsync_offset = pio_add_program(pio0, &vsync_program);
    rgb_offset = pio_add_program(pio0, &rgb320_program);
    printf("hsync_offset = %d\n", hsync_offset);
    printf("vsync_offset = %d\n", vsync_offset);
    printf("rgb_offset = %d\n", rgb_offset);

    // Manually select a few state machines from pio instance pio0.
    // initialize programs (C function in pio files)
    hsync_program_init(pio0, HSYNC_SM, hsync_offset, HSYNC);
    vsync_program_init(pio0, VSYNC_SM, vsync_offset, VSYNC);
    rgb320_program_init(pio0, RGB_SM, rgb_offset, LO_GRN);

    // Initialize PIO state machine counters.
    pio_sm_put_blocking(pio0, HSYNC_SM, H_ACTIVE);
    pio_sm_put_blocking(pio0, VSYNC_SM, V_ACTIVE);
    pio_sm_put_blocking(pio0, RGB_SM, (screen_width / 2) - 1);

    // DMA channels - 0 sends color data, 1 reconfigures and restarts 0
    rgb_chan_0 = dma_claim_unused_channel(true);
    printf("rgb_chan_0 = %d\n", rgb_chan_0);
    rgb_chan_1 = dma_claim_unused_channel(true);
    printf("rgb_chan_1 = %d\n", rgb_chan_1);

    init_dma();

    pio_enable_sm_mask_in_sync(pio0, ((1u << HSYNC_SM) | (1u << VSYNC_SM) | (1u << RGB_SM)));

    text::init();

    printf("VGA initialized.\n");
}

}
