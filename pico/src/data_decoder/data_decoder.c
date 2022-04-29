#include "data_decoder.h"

#include <stdio.h>

#define LANE_STATE_REQ_CMD       (1)
#define DEBUG_CMD                (2)
#define SET_PARAMETER_CMD        (3)
#define GET_PARAMETER_CMD        (4)

#define LANE_STATE_REQ_CMD_STOP         (1)
#define LANE_STATE_REQ_CMD_GO           (2)
#define LANE_STATE_REQ_CMD_OUT_OF_ORDER (3)

#define RIGHT_LANE_STATE_REQ_INDEX      (0)
#define CENTER_LANE_STATE_REQ_INDEX     (1)
#define LEFT_LANE_STATE_REQ_INDEX       (2)

static command_id_t decode_message_id(uint8_t command_id)
{
    switch (command_id)
    {
    case LANE_STATE_REQ_CMD:
        return COMMAND_ID_REQUEST_STATE;
    case DEBUG_CMD:
        return COMMAND_ID_DEBUG;
    case SET_PARAMETER_CMD:
        return COMMAND_ID_SET_PARAM;
    case GET_PARAMETER_CMD:
        return COMMAND_ID_GET_PARAM;
    default:
        break;
    }

    return COMMAND_ID_UNKNOWN;
}

static cmd_lane_state_req_t decode_raw_lane_state(uint32_t raw_state)
{
    switch (raw_state)
    {
    case LANE_STATE_REQ_CMD_GO:
        return CMD_LANE_STATE_REQ_GO;
    case LANE_STATE_REQ_CMD_STOP:
        return CMD_LANE_STATE_REQ_STOP;
    case LANE_STATE_REQ_CMD_OUT_OF_ORDER:
        return CMD_LANE_STATE_REQ_OUT_OF_ORDER;
    default:
        break;
    }

    return CMD_LANE_STATE_REQ_UNKNOWN;
}

static cmd_lane_state_req_t decode_lane_state_request(uint64_t raw_data,
                                                      uint8_t index)
{
    uint8_t* data = (uint8_t*)&raw_data;
    return decode_raw_lane_state(data[index]);
}

static command_t decode_request_state_message(uint64_t raw_data)
{
    command_t message;
    message.id = COMMAND_ID_REQUEST_STATE;

    message.request_state.left_lane_state =
        decode_lane_state_request(raw_data, LEFT_LANE_STATE_REQ_INDEX);

    message.request_state.center_lane_state =
        decode_lane_state_request(raw_data, CENTER_LANE_STATE_REQ_INDEX);

    message.request_state.right_lane_state =
        decode_lane_state_request(raw_data, RIGHT_LANE_STATE_REQ_INDEX);

    return message;
}

static command_t decode_unknown_message(void)
{
    return (command_t){ .id = COMMAND_ID_UNKNOWN };
}

static void printhex(raw_command_t raw_command)
{
    uint8_t* ptr = (uint8_t*)&raw_command;
    printf("raw: ");
    for (int i = 0; i < sizeof(raw_command_t); ++i)
    {
        printf(":%02x", ptr[i]);
    }
    printf("\n");
}

command_t data_decoder_decode_command(raw_command_t raw_command)
{
    printhex(raw_command);
    command_id_t command_id = decode_message_id(raw_command.command_id);

    switch (command_id)
    {
    case COMMAND_ID_REQUEST_STATE:
        printf("State request command\n");
        return decode_request_state_message(raw_command.data);
    case COMMAND_ID_DEBUG:
    case COMMAND_ID_SET_PARAM:
    case COMMAND_ID_GET_PARAM:
    case COMMAND_ID_UNKNOWN:
    default:
        break;
    }

    printf("Unknown command\n");
    return decode_unknown_message();
}
