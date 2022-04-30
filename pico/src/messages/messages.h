#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/* raw frames used on SPI bus */

typedef struct {
    uint8_t frame_id;
    uint8_t checksum;
    uint8_t reserved;
    uint8_t command_id;
    uint64_t data;
} raw_command_t;

typedef struct {
    uint8_t frame_id;
    uint8_t checksum;
    uint8_t reserved;
    uint8_t response_id;
    uint64_t data;
} raw_response_t;

static_assert(sizeof(raw_command_t) == sizeof(raw_response_t), "raw_command_t and raw_response_t must be of same size");

/* decoded messages used internally */

typedef enum
{
    COMMAND_ID_DEBUG,
    COMMAND_ID_SET_PARAM,
    COMMAND_ID_GET_PARAM,
    COMMAND_ID_REQUEST_STATE,
    COMMAND_ID_UNKNOWN
} command_id_t;



typedef enum {
    CMD_LANE_STATE_REQ_STOP,
    CMD_LANE_STATE_REQ_GO,
    CMD_LANE_STATE_REQ_OUT_OF_ORDER,
    CMD_LANE_STATE_REQ_UNKNOWN
} cmd_lane_state_req_t;

typedef struct {
    cmd_lane_state_req_t left_lane_state;
    cmd_lane_state_req_t center_lane_state;
    cmd_lane_state_req_t right_lane_state;
} request_state_command_t;

typedef struct {
    bool right_lane_red_on;
    bool right_lane_yellow_on;
    bool right_lane_green_on;
    bool center_lane_red_on;
    bool center_lane_yellow_on;
    bool center_lane_green_on;
    bool left_lane_red_on;
    bool left_lane_yellow_on;
    bool left_lane_green_on;
} debug_command_t;

typedef struct {

} set_param_command_t;

typedef struct {

} get_param_command_t;

typedef struct {
    command_id_t id;

    union {
        request_state_command_t request_state;
        debug_command_t debug;
        set_param_command_t set_param;
        get_param_command_t get_param;
    };
} command_t;

typedef enum {
    RESPONSE_ID_UNKNOWN,
    RESPONSE_ID_LANE_STATES,
    RESPONSE_ID_DEBUG,
    RESPONSE_ID_PARAMETER_VALUE
} response_id_t;

typedef enum {
    RESPONSE_LANE_STATE_UNKNOWN,
    RESPONSE_LANE_STATE_NONE,
    RESPONSE_LANE_STATE_STOP,
    RESPONSE_LANE_STATE_TO_GO,
    RESPONSE_LANE_STATE_GO,
    RESPONSE_LANE_STATE_TO_STOP,
    RESPONSE_LANE_STATE_OUT_OF_ORDER
} response_lane_state_t;

typedef struct {
    response_lane_state_t left_lane_state;
    bool left_lane_cars_on_lane;

    response_lane_state_t center_lane_state;
    bool center_lane_cars_on_lane;

    response_lane_state_t right_lane_state;
    bool right_lane_cars_on_lane;
} response_lane_states_t;

typedef struct {

} response_parameter_value_t;

typedef struct {
    response_id_t id;

    union {
        response_lane_states_t lane_states;
        response_parameter_value_t parameter_value;
    };
} response_t;
