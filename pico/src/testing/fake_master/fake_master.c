
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

#define CMD_REQUEST_STATE           (1)
#define CMD_STATE_STOP              (1)
#define CMD_STATE_GO                (2)
#define CMD_STATE_OUT_OF_ORDER      (3)

static uint test_phase = 0;

static uint8_t tx_frame_id = 0;
static uint32_t last_phase_change_ms = 0;

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

raw_command_t make_state_req_command(uint8_t left_state,
                                     uint8_t center_state,
                                     uint8_t right_state)
{
    raw_command_t raw_command = {0};

    raw_command.frame_id = tx_frame_id++;
    raw_command.command_id = CMD_REQUEST_STATE;

    uint8_t* data = (uint8_t*)&raw_command.data;

    data[0] = right_state;
    data[1] = center_state;
    data[2] = left_state;

    return raw_command;
}

bool run_test_light_sequence(struct repeating_timer* timer)
{
    (void)timer;

    if (clock_get_ms() - last_phase_change_ms > 10000)
    {
        ++test_phase;
        if (test_phase >= 6)
        {
            test_phase = 0;
        }
        last_phase_change_ms = clock_get_ms();
    }

    switch (test_phase)
    {
    case 0:
        send_spi_data(make_state_req_command(CMD_STATE_STOP,
                                             CMD_STATE_STOP,
                                             CMD_STATE_STOP));
        break;
    case 1:
        send_spi_data(make_state_req_command(CMD_STATE_GO,
                                             CMD_STATE_STOP,
                                             CMD_STATE_STOP));
        break;
    case 2:
        send_spi_data(make_state_req_command(CMD_STATE_GO,
                                             CMD_STATE_GO,
                                             CMD_STATE_STOP));
        break;
    case 3:
        send_spi_data(make_state_req_command(CMD_STATE_GO,
                                             CMD_STATE_GO,
                                             CMD_STATE_GO));
        break;
    case 4:
        send_spi_data(make_state_req_command(CMD_STATE_STOP,
                                             CMD_STATE_GO,
                                             CMD_STATE_GO));
        break;
    case 5:
        send_spi_data(make_state_req_command(CMD_STATE_STOP,
                                             CMD_STATE_GO,
                                             CMD_STATE_STOP));
        break;
    case 6:
        send_spi_data(make_state_req_command(CMD_STATE_OUT_OF_ORDER,
                                             CMD_STATE_OUT_OF_ORDER,
                                             CMD_STATE_OUT_OF_ORDER));
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
    add_repeating_timer_ms(1000, &run_test_light_sequence, 0, &test_timer);

    repeating_timer_t led_timer;
    add_repeating_timer_ms(500, &blink_led, 0, &led_timer);

    send_spi_data(make_state_req_command(CMD_STATE_STOP,
                                         CMD_STATE_STOP,
                                         CMD_STATE_STOP));

    while (true)
    {
    }
}
