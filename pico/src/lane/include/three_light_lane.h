#pragma once

#include "lane.h"

#include "pico/stdlib.h"

typedef struct {
    lane_base_t base;
    lane_state_t state;

    uint red_light;
    uint yellow_light;
    uint green_light;

    repeating_timer_t out_of_order_blink_timer;
    alarm_id_t state_transition_alarm;

} three_light_lane_t;

lane_t* three_light_lane_init(three_light_lane_t* three_light_lane,
                              uint red_light,
                              uint yellow_light,
                              uint green_light);