#include "lane_group.h"
#include "lane.h"
#include "data_comm.h"
#include "data_decoder.h"

#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <stdio.h>

#define ONBOARD_LED_GPIO        (25)
#define HEARTBEAT_INTERVAL_MS   (430)

static bool blink_onboard_led(struct repeating_timer* timer)
{
    (void)timer;
    gpio_put(ONBOARD_LED_GPIO, !gpio_get(ONBOARD_LED_GPIO));
    return true;
}

int main()
{
    stdio_init_all();

    gpio_init(ONBOARD_LED_GPIO);
    gpio_set_dir(ONBOARD_LED_GPIO, GPIO_OUT);

    data_comm_t data_comm;
    lane_group_t light_group;
    repeating_timer_t heartbeat_timer;

    lane_t left_lane;
    lane_t center_lane;
    lane_t right_lane;

    lane_init(&left_lane, 0, 0, 16);
    lane_init(&center_lane, 22, 19, 18);
    lane_init(&right_lane, 21, 20, 17);

    lane_group_config_t config = {
        .left_lane = &left_lane,
        .center_lane = &center_lane,
        .right_lane = &right_lane,
    };

    data_comm_init(&data_comm, spi0, 0, 3, 2, 1);
    lane_group_init(&light_group, &config);

    add_repeating_timer_ms(HEARTBEAT_INTERVAL_MS,
                           &blink_onboard_led,
                           0,
                           &heartbeat_timer);

    printf("Initialization done. Starting...\n");

    raw_response_t raw_response = {0};

    while (true) {
        raw_command_t raw_command;

        bool command_received = data_comm_exchange_raw_data(&data_comm,
                                                            &raw_command,
                                                            &raw_response);

        if (command_received)
        {
            printf("Handling command\n");
            command_t command = data_decoder_decode_command(raw_command);
            lane_group_handle_command(&light_group, command);
        }
    }

}