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

static command_t decode_request_state_command(raw_command_t raw_command)
{
    command_t message;
    message.id = COMMAND_ID_REQUEST_STATE;

    message.request_state.left_lane_state =
        decode_lane_state_request(raw_command.data, LEFT_LANE_STATE_REQ_INDEX);

    message.request_state.center_lane_state =
        decode_lane_state_request(raw_command.data, CENTER_LANE_STATE_REQ_INDEX);

    message.request_state.right_lane_state =
        decode_lane_state_request(raw_command.data, RIGHT_LANE_STATE_REQ_INDEX);

    return message;
}

static bool is_bit_set(uint64_t data, uint8_t bit_index)
{
    return (data & (1U << bit_index)) != 0U;
}

static command_t decode_debug_command(raw_command_t raw_command)
{
    command_t command = { .id = COMMAND_ID_DEBUG };

    command.debug.right_lane_red_on = is_bit_set(raw_command.data, 0);
    command.debug.right_lane_yellow_on = is_bit_set(raw_command.data, 1);
    command.debug.right_lane_green_on = is_bit_set(raw_command.data, 2);

    command.debug.center_lane_red_on = is_bit_set(raw_command.data, 3);
    command.debug.center_lane_yellow_on = is_bit_set(raw_command.data, 4);
    command.debug.center_lane_green_on = is_bit_set(raw_command.data, 5);

    command.debug.left_lane_red_on = is_bit_set(raw_command.data, 6);
    command.debug.left_lane_yellow_on = is_bit_set(raw_command.data, 7);
    command.debug.left_lane_green_on = is_bit_set(raw_command.data, 8);

    return command;
}

static command_t decode_unknown_message(void)
{
    return (command_t){ .id = COMMAND_ID_UNKNOWN };
}

command_t data_decoder_decode_command(raw_command_t raw_command)
{
    command_id_t command_id = decode_message_id(raw_command.command_id);

    switch (command_id)
    {
    case COMMAND_ID_REQUEST_STATE:
        return decode_request_state_command(raw_command);
    case COMMAND_ID_DEBUG:
        return decode_debug_command(raw_command);
    case COMMAND_ID_SET_PARAM:
    case COMMAND_ID_GET_PARAM:
    case COMMAND_ID_UNKNOWN:
    default:
        break;
    }

    printf("Unknown command\n");
    return decode_unknown_message();
}
