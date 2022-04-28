#pragma once

#include "pico/stdlib.h"

typedef enum {
    LANE_STATE_REQ_NONE,
    LANE_STATE_REQ_STOP,
    LANE_STATE_REQ_GO,
    LANE_STATE_REQ_OUT_OF_ORDER
} lane_state_req_t;

typedef enum {
    LANE_STATE_NONE,
    LANE_STATE_STOP,
    LANE_STATE_TRANSITION_TO_GO,
    LANE_STATE_GO,
    LANE_STATE_TRANSITION_TO_STOP,
    LANE_STATE_OUT_OF_ORDER
} lane_state_t;

typedef struct {
    lane_state_t state;
    lane_state_req_t target_req_state;

    uint red_light;
    uint yellow_light;
    uint green_light;

    repeating_timer_t out_of_order_blink_timer;
    alarm_id_t state_transition_alarm;
} lane_t;

void lane_init(lane_t* lane,
               uint red_light,
               uint yellow_light,
               uint green_light);

void lane_request_state(lane_t* lane, lane_state_req_t state_req);

lane_state_t lane_get_state(lane_t* lane);