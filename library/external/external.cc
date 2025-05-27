#include "external.hh"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "fortuna.hh"

namespace external {

void init(uint32_t speed)
{
    spi_init(spi0, speed);
    spi_set_slave(spi0, true);
    gpio_set_function(2, GPIO_FUNC_SPI);
    gpio_set_function(3, GPIO_FUNC_SPI);
    gpio_set_function(4, GPIO_FUNC_SPI);
    gpio_set_function(5, GPIO_FUNC_SPI);

    irq_set_enabled(SPI0_IRQ, true);
    irq_set_exclusive_handler(SPI0_IRQ, []() {
        uint8_t recv, send = 'A';
        spi_write_read_blocking(spi0, &recv, &send, 1);
        fortuna::add_event(fortuna::Event {
            .type = fortuna::Event::Type::External,
            .buffer = { recv },
        });
    });
    spi0_hw->imsc = SPI_SSPIMSC_RTIM_BITS | SPI_SSPIMSC_RORIM_BITS | SPI_SSPIMSC_RXIM_BITS;
}

void step()
{

}



}
