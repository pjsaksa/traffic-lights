
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

#define LED             (25)

#define SPI             (spi0)
#define SPI_RX          (0)
#define SPI_CS          (1)
#define SPI_SCK         (2)
#define SPI_TX          (3)

int main(void)
{
    stdio_init_all();

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CS, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);

    spi_init(SPI, 500000);
    spi_set_slave(SPI, true);
    spi_set_format(SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    while (true)
    {
        if (spi_is_readable(SPI))
        {
            uint8_t data = 0;

            int bytes_read = spi_read_blocking(SPI, 0, &data, 1);

            if (bytes_read > 0)
            {
                gpio_put(LED, !gpio_get(LED));
                printf("Data read: %d\n", data);
            }
        }
    }
}