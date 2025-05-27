#include "external.hh"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "fortuna.hh"

namespace external {

void init()
{
    spi_init(spi0, 1000000);
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_slave(spi0, true);
    gpio_set_function(2, GPIO_FUNC_SPI);
    gpio_set_function(3, GPIO_FUNC_SPI);
    gpio_set_function(4, GPIO_FUNC_SPI);
    gpio_set_function(5, GPIO_FUNC_SPI);

    irq_set_enabled(SPI0_IRQ, true);
    irq_set_exclusive_handler(SPI0_IRQ, []() {
        while (spi_is_readable(spi0)) {
            uint64_t i = 0;
            fortuna::Event e = {
                .type = fortuna::Event::Type::External,
                .external = { .sz = 0 },
            };
            while (spi_is_readable(spi0) && i < BUFFER_RECV_SZ) {
                spi_read_blocking(spi0, 1, &e.external.data[i++], 1);
                e.external.sz = i;
            }
            if (e.external.sz > 0)
                fortuna::add_event(e);
        }
    });
    spi0_hw->imsc = SPI_SSPIMSC_RXIM_BITS | SPI_SSPIMSC_RTIM_BITS;
}

void step()
{
    /*
    if (spi_is_readable(spi0)) {
        uint8_t i = 0;
        fortuna::Event e = {
            .type = fortuna::Event::Type::External,
            .external = Buffer(),
        };
        while (spi_is_readable(spi0)  && i < BUFFER_RECV_SZ) {
            uint8_t recv = spi_get_hw(spi0)->dr;
            e.external.data[i++] = (uint8_t) spi0_hw->dr;
            e.external.sz = i;
        }
        if (e.external.sz > 0)
            fortuna::add_event(e);
    }
    */
}

}
