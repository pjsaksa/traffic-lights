
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

#define LED             (25)

#define SPI             (spi0)
#define SPI_RX          (16)
#define SPI_CS          (17)
#define SPI_SCK         (18)
#define SPI_TX          (19)

static uint8_t data = 0;

bool send_spi_data(struct repeating_timer* timer)
{
    (void)timer;

    if (spi_is_writable(SPI))
    {
        printf("Writing to SPI %d\n", data);
        spi_write_blocking(SPI, &data, sizeof(data));
        ++data;
    }

    return true;
}

bool blink_led(struct repeating_timer* timer)
{
    (void)timer;

    gpio_put(LED, !gpio_get(LED));

    return true;
}

int main(void)
{
    stdio_init_all();

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CS, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);

    spi_init(SPI, 500000);
    spi_set_format(SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    repeating_timer_t spi_timer;
    add_repeating_timer_ms(1000, &send_spi_data, 0, &spi_timer);

    repeating_timer_t led_timer;
    add_repeating_timer_ms(500, &blink_led, 0, &led_timer);

    while (true)
    {

    }
}
