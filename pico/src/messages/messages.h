#pragma once

#include <stdint.h>
#include <assert.h>

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

typedef enum
{
    COMMAND_ID_DEBUG,
    COMMAND_ID_SET_PARAM,
    COMMAND_ID_GET_PARAM,
    COMMAND_ID_REQUEST_STATE,
    COMMAND_ID_UNKNOWN
} command_id_t;

typedef struct {

} debug_command_t;

typedef struct {

} set_param_command_t;

typedef struct {

} get_param_command_t;

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
    command_id_t id;

    union {
        debug_command_t debug;
        set_param_command_t set_param;
        get_param_command_t get_param;
        request_state_command_t request_state;
    };
} command_t;
