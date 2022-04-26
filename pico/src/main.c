#include "traffic_light_group.h"
#include "three_light_lane.h"
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
    traffic_light_group_t light_group;
    repeating_timer_t heartbeat_timer;

    three_light_lane_t left_tllane;
    three_light_lane_t center_tllane;
    three_light_lane_t right_tllane;

    lane_t* left_lane = three_light_lane_init(&left_tllane, 0, 0, 16);
    lane_t* center_lane = three_light_lane_init(&center_tllane, 22, 19, 18);
    lane_t* right_lane = three_light_lane_init(&right_tllane, 21, 20, 17);

    traffic_light_group_config_t config = {
        .left_lane = left_lane,
        .center_lane = center_lane,
        .right_lane = right_lane,
    };

    data_comm_init(&data_comm, spi0, 0, 3, 2, 1);
    traffic_light_group_init(&light_group, &config);

    add_repeating_timer_ms(HEARTBEAT_INTERVAL_MS,
                           &blink_onboard_led,
                           0,
                           &heartbeat_timer);

    printf("Initialization done. Starting...\n");

    while (true) {
        uint32_t raw_data = data_comm_exchange_raw_data(&data_comm, 0);
        message_t message = data_decoder_handle_message(raw_data);
        traffic_light_group_execute(&light_group, message);
    }

}