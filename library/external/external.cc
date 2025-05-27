#include "external.hh"

#include "pico/stdlib.h"
#include "hardware/spi.h"

namespace external {

void init(uint32_t speed)
{
    spi_init(spi0, speed);
    spi_set_slave(spi0, true);
    gpio_set_function(2, GPIO_FUNC_SPI);
    gpio_set_function(3, GPIO_FUNC_SPI);
    gpio_set_function(4, GPIO_FUNC_SPI);
    gpio_set_function(5, GPIO_FUNC_SPI);
}

void step()
{

}



}
