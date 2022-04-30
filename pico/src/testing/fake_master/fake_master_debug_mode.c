
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "messages.h"
#include "clock.h"

#include <stdio.h>

#define LED             (25)

#define SPI             (spi0)
#define SPI_RX          (16)
#define SPI_CS          (17)
#define SPI_SCK         (18)
#define SPI_TX          (19)

#define CMD_DEBUG       (2)

static uint8_t tx_frame_id = 0;


static void send_spi_data(raw_command_t command)
{
    raw_response_t response;

    spi_write_read_blocking(SPI,
                            (const uint8_t*) &command,
                            (uint8_t*) &response,
                            sizeof(command));

    printf("*******************************\n");
    printf("Tx: id: %d, cmd: %d, data: %llx\n",
           command.frame_id,
           command.command_id,
           command.data);

    printf("Rx: id: %d, rsp: %d, data: %llx\n",
           response.frame_id,
           response.response_id,
           response.data);
}

void toggle_bit(uint64_t* data, uint8_t bit_index)
{
    *data = *data ^ (1U << bit_index);
}

raw_command_t make_debug_command(void)
{
    static uint8_t bit_index = 0;
    static uint64_t debug_commands = 0;

    raw_command_t raw_command = {0};

    raw_command.frame_id = tx_frame_id++;
    raw_command.command_id = CMD_DEBUG;

    toggle_bit(&debug_commands, bit_index++);

    if (bit_index >= 9)
    {
        bit_index = 0;
    }

    raw_command.data = debug_commands;

    return raw_command;
}

bool run_light_debug_sequence(struct repeating_timer* timer)
{
    (void)timer;

    send_spi_data(make_debug_command());

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
    add_repeating_timer_ms(500, &run_light_debug_sequence, 0, &test_timer);

    repeating_timer_t led_timer;
    add_repeating_timer_ms(500, &blink_led, 0, &led_timer);

    while (true)
    {
    }
}
