#pragma once

#include <stdint.h>

typedef enum
{
    MESSAGE_ID_DEBUG,
    MESSAGE_ID_SET_PARAM,
    MESSAGE_ID_GET_PARAM,
    MESSAGE_ID_REQUEST_STATE,
    MESSAGE_ID_UNKNOWN
} message_id_t;

typedef struct {

} debug_message_t;

typedef struct {

} set_param_message_t;

typedef struct {

} get_param_message_t;

typedef enum {
    MESSAGE_STATE_REQ_STOP,
    MESSAGE_STATE_REQ_GO,
    MESSAGE_STATE_REQ_UNKNOWN
} message_state_req_t;

typedef struct {

    struct {
        message_state_req_t requested_state;
    } left_lane;

    struct {
        message_state_req_t requested_state;
    } center_lane;

    struct {
        message_state_req_t requested_state;
    } right_lane;

} request_state_message_t;

typedef struct {
    message_id_t id;

    union {
        debug_message_t debug;
        set_param_message_t set_param;
        get_param_message_t get_param;
        request_state_message_t request_state;
    };
} message_t;
