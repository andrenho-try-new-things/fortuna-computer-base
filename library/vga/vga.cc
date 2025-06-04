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

// Pixel color array that is DMA's to the PIO machines and
// a pointer to the ADDRESS of this color array.
// Note that this array is automatically initialized to all 0's (black)
// Bit masks for drawPixel routine
#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

namespace vga {

// VGA framebuffer
uint8_t* vga_data_array;
static void* address_pointer = nullptr;

// DMA channels and PIO program offsets
static int rgb_chan_0, rgb_chan_1;
static uint rgb_offset, vsync_offset, hsync_offset;

// state machines
constexpr uint HSYNC_SM = 0;
constexpr uint VSYNC_SM = 1;
constexpr uint RGB_SM = 2;

// current cannon location
static uint16_t current_scanline = 0;
static uint8_t  current_framebuffer = 0;
static volatile bool new_vsync = false;

// framebuffer size in mode 4
constexpr uint32_t FRAMEBUFFER_SZ = 320 * 240 / 2;

// current state
static Mode current_mode = Mode::V_640x480;
static pio_program const* current_rgb_program = nullptr;

// current screen size
uint16_t screen_width = 640;
uint16_t screen_height = 480;

// sprites
static Sprite*  sprites = nullptr;
static uint16_t sprite_sz = 0;

// mouse location
static constexpr uint8_t CURSOR_HEIGHT = 8;
static int8_t next_mouse_x = 0, next_mouse_y = 0;
static uint16_t mouse_x = 0, mouse_y = 0;
static uint8_t vga_data_array_mouse[640 * CURSOR_HEIGHT / 2];
bool show_mouse_pointer = false;

static inline __attribute__((always_inline)) uint32_t pixel_idx(uint16_t x, uint16_t y, uint8_t framebuffer=0)
{
    switch (current_mode) {
        case Mode::V_640x480:
            return (640 >> 1) * y + (x >> 1);
        case Mode::V_640x240:
            return (640 >> 1) * (y >> 1) + (x >> 1);
        case Mode::V_320x240:
            return (320 >> 1) * (y >> 1) + (x >> 1);
        case Mode::V_SPRITES:
            return (320 >> 1) * (y >> 1) + (x >> 1) + (framebuffer * FRAMEBUFFER_SZ);
    }
    return 0;
}

static void dma_handler()   // DMA handler is called at the end of each HSYNC
{
    // Clear the interrupt request for DMA control channel
    dma_hw->ints0 = (1u << rgb_chan_0);

    // increment scanline (1..)
    current_scanline++;                  // new current scanline
    if (current_scanline >= 480) {       // last scanline?
        current_scanline = 0;            // restart scanline

        if (current_mode == Mode::V_SPRITES)
            current_framebuffer = (current_framebuffer == 1) ? 2 : 1;

        new_vsync = true;
    }

    int16_t mouse_diff = current_scanline - (int) mouse_y;
    if (show_mouse_pointer && mouse_diff >= 0 && mouse_diff < CURSOR_HEIGHT)
        address_pointer = &vga_data_array_mouse[pixel_idx(0, mouse_diff)];
    else
        address_pointer = &vga_data_array[pixel_idx(0, current_scanline, current_framebuffer)];
}


static void init_dma()
{
    // Channel Zero (sends color data to PIO VGA machine)
    dma_channel_config c0 = dma_channel_get_default_config(rgb_chan_0);  // default configs
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
    dma_channel_config c1 = dma_channel_get_default_config(rgb_chan_1);   // default configs
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


static void _set_mode(Mode mode)
{
    // wait for DMA to finish
    dma_channel_wait_for_finish_blocking(rgb_chan_1);
    dma_channel_wait_for_finish_blocking(rgb_chan_0);

    // disable PIO programs
    pio_sm_set_enabled(pio0, RGB_SM, false);
    pio_sm_set_enabled(pio0, VSYNC_SM, false);
    pio_sm_set_enabled(pio0, HSYNC_SM, false);

    // set mode, and replace RGB program
    pio_program_t const* new_program = nullptr;
    switch (mode) {
        case Mode::V_640x480:
            screen_width = 640;
            screen_height = 480;
            new_program = &rgb640_program;
            current_framebuffer = 0;
            break;
        case Mode::V_320x240:
            screen_width = 320;
            screen_height = 240;
            new_program = &rgb320_program;
            current_framebuffer = 0;
            break;
        case Mode::V_640x240:
            screen_width = 640;
            screen_height = 240;
            new_program = &rgb640_program;
            current_framebuffer = 0;
            break;
        case Mode::V_SPRITES:
            screen_width = 320;
            screen_height = 240;
            new_program = &rgb320_program;
            current_framebuffer = 1;
            break;
    }
    current_mode = mode;
    free(vga_data_array);
    vga_data_array = (uint8_t *) calloc(1, screen_width * screen_height / 2 * (mode == Mode::V_SPRITES ? 3 : 1));
    address_pointer = &vga_data_array[0];

    // update mouse pos
    mouse_x = MIN(mouse_x, screen_width - 1);
    mouse_y = MIN(mouse_y, screen_height - 1);

    // replace RGB PIO program
    pio_remove_program(pio0, current_rgb_program, rgb_offset);
    rgb_offset = pio_add_program(pio0, new_program);
    current_rgb_program = new_program;

    // reset data to feed the PIO programs
    pio_sm_clear_fifos(pio0, RGB_SM);
    pio_sm_put_blocking(pio0, RGB_SM, (screen_width / 2) - 1);
    pio_sm_exec(pio0, RGB_SM, pio_encode_jmp(rgb_offset));

    pio_sm_clear_fifos(pio0, HSYNC_SM);
    pio_sm_put_blocking(pio0, HSYNC_SM, H_ACTIVE);
    pio_sm_exec(pio0, HSYNC_SM, pio_encode_jmp(hsync_offset));

    pio_sm_clear_fifos(pio0, VSYNC_SM);
    pio_sm_put_blocking(pio0, VSYNC_SM, V_ACTIVE);
    pio_sm_exec(pio0, VSYNC_SM, pio_encode_jmp(vsync_offset));

    // update DMA counter
    current_scanline = 0;
    dma_channel_set_trans_count(rgb_chan_0, screen_width / 2, false);

    // reinitialize programs in sync
    pio_enable_sm_mask_in_sync(pio0, ((1u << HSYNC_SM) | (1u << VSYNC_SM) | (1u << RGB_SM)));

    // adjust text matrix
    text::recalculate_matrix_size();
}

void set_mode(Mode mode)
{
    for (uint8_t i = 0; i < 2; ++i) {   // I don't know why, it only works when executed twice
        _set_mode(mode);
        sleep_ms(50);
    }
}

void initialize_pio()
{
    // load PIO programs
    hsync_offset = pio_add_program(pio0, &hsync_program);
    vsync_offset = pio_add_program(pio0, &vsync_program);
    rgb_offset = pio_add_program(pio0, &rgb640_program);
    current_rgb_program = &rgb640_program;

    // initialize PIO programs
    hsync_program_init(pio0, HSYNC_SM, hsync_offset, HSYNC);
    vsync_program_init(pio0, VSYNC_SM, vsync_offset, VSYNC);
    rgb640_program_init(pio0, RGB_SM, rgb_offset, LO_GRN);

    // initialize PIO state machine counters.
    pio_sm_put_blocking(pio0, HSYNC_SM, H_ACTIVE);
    pio_sm_put_blocking(pio0, VSYNC_SM, V_ACTIVE);
    pio_sm_put_blocking(pio0, RGB_SM, (screen_width / 2) - 1);

    // claim DMA channels
    rgb_chan_0 = dma_claim_unused_channel(true);
    rgb_chan_1 = dma_claim_unused_channel(true);

    // initialize DMA
    init_dma();

    // initialize PIO programs in sync
    pio_enable_sm_mask_in_sync(pio0, ((1u << HSYNC_SM) | (1u << VSYNC_SM) | (1u << RGB_SM)));
}

void init()
{
    // initialize in 640x480
    screen_width = 640;
    screen_height = 480;
    current_mode = Mode::V_640x480;

    vga_data_array = (uint8_t *) calloc(1, screen_width * screen_height / 2);
    address_pointer = &vga_data_array[0];

    initialize_pio();

    text::init();

    printf("VGA initialized.\n");
}

void update_mouse_pointer()
{
    if (show_mouse_pointer) {
        mouse_x = MIN(MAX(mouse_x + next_mouse_x, 0), screen_width - 1);
        mouse_y = MIN(MAX(mouse_y + next_mouse_y, 0), screen_height - 1);
        next_mouse_x = 0;
        next_mouse_y = 0;

        // copy mouse lines onto mouse buffer
        memcpy(vga_data_array_mouse, &vga_data_array[pixel_idx(0, mouse_y, current_framebuffer)], (CURSOR_HEIGHT * screen_width) >> 1);

        // add mouse to mouse array
        vga_data_array_mouse[mouse_x >> 1] = 0xff;
    }
}

void copy_sprites_vsync()
{
    if (current_mode == Mode::V_SPRITES) {
        // copy framebuffer 0 on top of current framebuffer
        uint8_t opposite_framebuffer = (current_framebuffer == 1) ? 2 : 1;
        memcpy(&vga_data_array[pixel_idx(0, 0, opposite_framebuffer)], &vga_data_array[0], FRAMEBUFFER_SZ);

        // add sprites
        for (uint16_t i = 0; i < sprite_sz; ++i)
            fb::draw_image(*sprites[i].image, sprites[i].x, sprites[i].y, opposite_framebuffer);
    }
}

void step()
{
    if (new_vsync) {

        update_mouse_pointer();
        copy_sprites_vsync();

        new_vsync = false;
    }
}

void set_sprites(Sprite* sprites, uint16_t sz)
{
    vga::sprites = sprites;
    vga::sprite_sz = sz;
}

void update_mouse_position(int8_t x, int8_t y)
{
    next_mouse_x = x;
    next_mouse_y = y;
}

}
