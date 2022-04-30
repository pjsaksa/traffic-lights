#include "lane_group.h"
#include "lane.h"
#include "data_comm.h"
#include "data_decoder.h"
#include "car_sensor.h"

#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <stdio.h>

#define ONBOARD_LED_GPIO        (25)
#define HEARTBEAT_INTERVAL_MS   (430)

#define SPI_RX_GPIO             (0)
#define SPI_TX_GPIO             (3)
#define SPI_CLK_GPIO            (2)
#define SPI_CS_GPIO             (1)

#define LEFT_LANE_SENSOR_GPIO   (15)
#define LEFT_LANE_RED_GPIO      (LANE_UNUSED_GPIO)
#define LEFT_LANE_YELLOW_GPIO   (LANE_UNUSED_GPIO)
#define LEFT_LANE_GREEN_GPIO    (16)

#define CENTER_LANE_SENSOR_GPIO (14)
#define CENTER_LANE_RED_GPIO    (22)
#define CENTER_LANE_YELLOW_GPIO (19)
#define CENTER_LANE_GREEN_GPIO  (18)

#define RIGHT_LANE_SENSOR_GPIO  (13)
#define RIGHT_LANE_RED_GPIO     (21)
#define RIGHT_LANE_YELLOW_GPIO  (20)
#define RIGHT_LANE_GREEN_GPIO   (17)

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

    car_sensor_t left_lane_car_sensor;
    car_sensor_t center_lane_car_sensor;
    car_sensor_t right_lane_car_sensor;

    car_sensor_init(&left_lane_car_sensor, LEFT_LANE_SENSOR_GPIO);
    car_sensor_init(&center_lane_car_sensor, CENTER_LANE_SENSOR_GPIO);
    car_sensor_init(&right_lane_car_sensor, RIGHT_LANE_SENSOR_GPIO);

    lane_t left_lane;
    lane_t center_lane;
    lane_t right_lane;

    lane_init(&left_lane,
              LEFT_LANE_RED_GPIO,
              LEFT_LANE_YELLOW_GPIO,
              LEFT_LANE_GREEN_GPIO,
              &left_lane_car_sensor);

    lane_init(&center_lane,
              CENTER_LANE_RED_GPIO,
              CENTER_LANE_YELLOW_GPIO,
              CENTER_LANE_GREEN_GPIO,
              &center_lane_car_sensor);

    lane_init(&right_lane,
              RIGHT_LANE_RED_GPIO,
              RIGHT_LANE_YELLOW_GPIO,
              RIGHT_LANE_GREEN_GPIO,
              &right_lane_car_sensor);

    lane_group_config_t config = {
        .left_lane = &left_lane,
        .center_lane = &center_lane,
        .right_lane = &right_lane,
    };

    data_comm_init(&data_comm,
                   spi0,
                   SPI_RX_GPIO,
                   SPI_TX_GPIO,
                   SPI_CLK_GPIO,
                   SPI_CS_GPIO);

    lane_group_init(&light_group, &config);

    add_repeating_timer_ms(HEARTBEAT_INTERVAL_MS,
                           &blink_onboard_led,
                           0,
                           &heartbeat_timer);

    printf("Initialization done. Starting...\n");

    raw_response_t raw_response = {0};

    while (true)
    {
        raw_command_t raw_command
            = data_comm_exchange_raw_data(&data_comm, raw_response);

        command_t command = data_decoder_decode_command(raw_command);
        lane_group_handle_command(&light_group, command);

        raw_response.frame_id = raw_command.frame_id;
        raw_response.response_id = 0;
        raw_response.data = 0;

    }

}