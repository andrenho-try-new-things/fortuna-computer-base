#include "external.hh"

#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <hardware/spi.h>

#include "fortuna.hh"

namespace external {

static uint16_t RESPONSE_QUEUE_SZ = 256;

static queue_t  response_queue;
static uint8_t  default_response = 0xff;

void init()
{
    queue_init(&response_queue, 1, RESPONSE_QUEUE_SZ);

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
                uint8_t next;
                if (!queue_try_remove(&response_queue, &next))
                    next = default_response;
                while (!spi_is_writable(spi0)) {}
                spi_write_read_blocking(spi0, &next, &e.external.data[i++], 1);
                e.external.sz = i;
            }
            if (e.external.sz > 0)
                fortuna::add_event(e);
        }
    });
    spi0_hw->imsc = SPI_SSPIMSC_RXIM_BITS | SPI_SSPIMSC_RTIM_BITS;
}

void add_response(uint8_t* data, size_t sz)
{
    for (uint16_t i = 0; i < sz; ++i)
        queue_try_add(&response_queue, &data[i]);
}

void change_default_response(uint8_t value)
{
    default_response = value;
}

void send_request_to_event_queue(bool value)
{
    irq_set_enabled(SPI0_IRQ, value);
}

}
