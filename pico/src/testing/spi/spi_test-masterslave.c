
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

/* Test firmware runs both master and slave SPI within one PICO.
 * Connect following GPIOs with a jumper cable
 *
 * GPIO16 (master rx)   <---> GPIO15 (slave tx)
 * GPIO17 (master cs)   <---> GPIO13 (slave cs)
 * GPIO18 (master clk)  <---> GPIO14 (slave clk)
 * GPIO19 (master tx)   <---> GPIO12 (slave rx)
 *
 */

#define MASTER_SPI              (spi0)
#define MASTER_SPI_RX           (16)
#define MASTER_SPI_CS           (17)
#define MASTER_SPI_SCK          (18)
#define MASTER_SPI_TX           (19)

#define SLAVE_SPI               (spi1)
#define SLAVE_SPI_RX            (12)
#define SLAVE_SPI_CS            (13)
#define SLAVE_SPI_SCK           (14)
#define SLAVE_SPI_TX            (15)

#define SPI_BAUDRATE            (500000)
#define SPI_DATABITS            (8)

#define LED                     (25)
#define LED_TIMER_MS            (500)

static inline uint32_t get_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}

static bool blink_led(struct repeating_timer* timer)
{
    (void)timer;

    gpio_put(LED, !gpio_get(LED));

    return true;
}

static void init_master(void)
{
    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    static repeating_timer_t led_timer;
    add_repeating_timer_ms(LED_TIMER_MS, &blink_led, 0, &led_timer);

    gpio_set_function(MASTER_SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(MASTER_SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(MASTER_SPI_CS, GPIO_FUNC_SPI);
    gpio_set_function(MASTER_SPI_SCK, GPIO_FUNC_SPI);

    spi_init(MASTER_SPI, SPI_BAUDRATE);
    spi_set_format(MASTER_SPI, SPI_DATABITS, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

static void init_slave(void)
{
    gpio_set_function(SLAVE_SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(SLAVE_SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(SLAVE_SPI_CS, GPIO_FUNC_SPI);
    gpio_set_function(SLAVE_SPI_SCK, GPIO_FUNC_SPI);

    spi_init(SLAVE_SPI, SPI_BAUDRATE);
    spi_set_slave(SLAVE_SPI, true);
    spi_set_format(SLAVE_SPI, SPI_DATABITS, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

static uint32_t master_last_send_ms = 0;

static void master_task()
{
    static uint8_t master_counter = 0;

    if (get_ms() - master_last_send_ms > 1000)
    {
        uint8_t rx_data;
        spi_write_read_blocking(MASTER_SPI,
                                &master_counter,
                                &rx_data,
                                sizeof(master_counter));
        printf("MASTER: Tx: %d, Rx: %d\n", master_counter, rx_data);
        master_last_send_ms = get_ms();
        ++master_counter;
    }

}

static void slave_task()
{
    static uint8_t slave_counter = 0;
    uint8_t rx_data;

    if (spi_is_readable(SLAVE_SPI))
    {
        spi_write_read_blocking(SLAVE_SPI, &slave_counter, &rx_data, sizeof(rx_data));
        printf("SLAVE: Rx: %d, Tx: %d\n", rx_data, slave_counter);
        printf("-------------------------\n");
        ++slave_counter;
    }
}

int main(void)
{
    stdio_init_all();

    init_master();
    init_slave();

    while (true)
    {
        master_task();
        slave_task();
    }
}