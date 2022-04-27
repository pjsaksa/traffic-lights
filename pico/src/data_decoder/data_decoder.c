#include "data_decoder.h"

#include <stdio.h>

#define REQUEST_STATE_CMD              (4)
#define REQUEST_STATE_CMD_STATE_STOP   (1)
#define REQUEST_STATE_CMD_STATE_GO     (2)

static message_id_t decode_message_id(uint32_t raw_message)
{
    uint8_t cmd_id = raw_message >> 24U;
    message_id_t message_id = MESSAGE_ID_UNKNOWN;

    if (cmd_id == REQUEST_STATE_CMD)
    {
        message_id = MESSAGE_ID_REQUEST_STATE;
    }

    return message_id;
}

static message_state_req_t decode_raw_state(uint32_t raw_state)
{
    switch (raw_state)
    {
    case REQUEST_STATE_CMD_STATE_GO:
        return MESSAGE_STATE_REQ_GO;
    case REQUEST_STATE_CMD_STATE_STOP:
        return MESSAGE_STATE_REQ_STOP;
    default:
        break;
    }

    return MESSAGE_STATE_REQ_UNKNOWN;
}

static message_state_req_t decode_left_lane_state_request(uint32_t raw_data)
{
    return decode_raw_state((raw_data >> 16U) & 0x000000FF);
}

static message_state_req_t decode_center_lane_state_request(uint32_t raw_data)
{
    return decode_raw_state((raw_data >> 8U) & 0x000000FF);
}

static message_state_req_t decode_right_lane_state_request(uint32_t raw_data)
{
    return decode_raw_state(raw_data & 0x000000FF);
}

static message_t decode_request_state_message(uint32_t raw_data)
{
    message_t request_state_msg;
    request_state_msg.id = MESSAGE_ID_REQUEST_STATE;

    request_state_msg.request_state.left_lane.requested_state =
        decode_left_lane_state_request(raw_data);

    request_state_msg.request_state.center_lane.requested_state =
        decode_center_lane_state_request(raw_data);

    request_state_msg.request_state.right_lane.requested_state =
        decode_right_lane_state_request(raw_data);

    return request_state_msg;
}

static message_t decode_unknown_message(void)
{
    return (message_t){ .id = MESSAGE_ID_UNKNOWN };
}

message_t data_decoder_handle_message(uint32_t raw_message)
{
    message_id_t message_id = decode_message_id(raw_message);

    switch (message_id)
    {
    case MESSAGE_ID_REQUEST_STATE:
        return decode_request_state_message(raw_message);
    case MESSAGE_ID_DEBUG:
    case MESSAGE_ID_SET_PARAM:
    case MESSAGE_ID_GET_PARAM:
    case MESSAGE_ID_UNKNOWN:
    default:
        break;
    }

    return decode_unknown_message();
}
