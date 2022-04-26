
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>

#define LED             (25)

#define SPI             (spi0)
#define SPI_RX          (16)
#define SPI_CS          (17)
#define SPI_SCK         (18)
#define SPI_TX          (19)

#define REQUEST_STATE_CMD              (4)
#define STATE_STOP   (1)
#define STATE_GO     (2)

static uint test_phase = 0;

static void send_spi_data(uint32_t data)
{
    uint32_t tx_data;

    if (spi_is_writable(SPI))
    {
        spi_write_read_blocking(SPI,
                                (const uint8_t*) &data,
                                (uint8_t*) &tx_data,
                                sizeof(data));
    }
}

uint32_t make_state_req_command(bool left_stop,
                                bool center_stop,
                                bool right_stop)
{
    uint32_t state_req = REQUEST_STATE_CMD << 24U;

    state_req |= (left_stop ? STATE_STOP : STATE_GO) << 16U;
    state_req |= (center_stop ? STATE_STOP : STATE_GO) << 8U;
    state_req |= (right_stop ? STATE_STOP : STATE_GO);

    return state_req;
}

bool run_test_light_sequence(struct repeating_timer* timer)
{
    (void)timer;

    switch (test_phase)
    {
    case 0:
        send_spi_data(make_state_req_command(false, true, true));
        ++test_phase;
        break;
    case 1:
        send_spi_data(make_state_req_command(false, true, false));
        ++test_phase;
        break;
    case 2:
        send_spi_data(make_state_req_command(false, false, false));
        ++test_phase;
        break;
    case 3:
        send_spi_data(make_state_req_command(true, false, false));
        ++test_phase;
        break;
    case 4:
        send_spi_data(make_state_req_command(true, true, true));
        test_phase = 0;
        break;
    default:
        break;
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

    repeating_timer_t test_timer;
    add_repeating_timer_ms(5000, &run_test_light_sequence, 0, &test_timer);

    repeating_timer_t led_timer;
    add_repeating_timer_ms(500, &blink_led, 0, &led_timer);

    send_spi_data(make_state_req_command(false, false, false));

    while (true)
    {
    }
}
