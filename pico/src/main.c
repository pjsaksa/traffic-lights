#include "pico/stdlib.h"

#include "traffic_light_group.h"
#include "three_light_lane.h"

#include <stdio.h>

#define ONBOARD_LED_GPIO        (25)
#define HEARTBEAT_INTERVAL_MS   (1000)

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

    traffic_light_group_t light_group;
    repeating_timer_t heartbeat_timer;

    three_light_lane_t center_tllane;
    three_light_lane_t right_tllane;

    lane_t* center_lane = three_light_lane_init(&center_tllane, 22, 19, 18);
    lane_t* right_lane = three_light_lane_init(&right_tllane, 21, 20, 17);

    traffic_light_group_config_t config = {
        .left_lane = 0,
        .center_lane = center_lane,
        .right_lane = right_lane,
    };

    traffic_light_group_init(&light_group, &config);

    add_repeating_timer_ms(HEARTBEAT_INTERVAL_MS,
                           &blink_onboard_led,
                           0,
                           &heartbeat_timer);

    printf("Initialization done. Starting...\n");

    while (true) {
        traffic_light_group_execute(&light_group);
    }

}