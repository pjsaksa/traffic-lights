#include "lane_group.h"
#include "lane.h"
#include "messages.h"

#include <pico/stdlib.h>

#define OUT_OF_ORDER_TIMEOUT_MS     (10000)

static void set_lane_out_of_order(lane_t* lane)
{
    if (lane)
    {
        lane_request_state(lane, LANE_STATE_REQ_OUT_OF_ORDER);
    }
}

static bool request_lane_state(lane_t* lane, cmd_lane_state_req_t new_state)
{
    bool success = true;

    if (lane)
    {
        switch (new_state)
        {
        case CMD_LANE_STATE_REQ_STOP:
            lane_request_state(lane, LANE_STATE_REQ_STOP);
            break;
        case CMD_LANE_STATE_REQ_GO:
            lane_request_state(lane, LANE_STATE_REQ_GO);
            break;
        case CMD_LANE_STATE_REQ_OUT_OF_ORDER:
            set_lane_out_of_order(lane);
            break;
        case CMD_LANE_STATE_REQ_UNKNOWN:
        default:
            success = false;
            break;
        }
    }

    return success;
}

static response_lane_state_t get_lane_state(lane_t* lane)
{
    lane_state_t state = lane_get_state(lane);

    switch (state)
    {
    case LANE_STATE_NONE:
        return RESPONSE_LANE_STATE_NONE;
    case LANE_STATE_STOP:
        return RESPONSE_LANE_STATE_STOP;
    case LANE_STATE_TRANSITION_TO_GO:
        return RESPONSE_LANE_STATE_TO_GO;
    case LANE_STATE_GO:
        return RESPONSE_LANE_STATE_GO;
    case LANE_STATE_TRANSITION_TO_STOP:
        return RESPONSE_LANE_STATE_TO_STOP;
    case LANE_STATE_OUT_OF_ORDER:
        return RESPONSE_LANE_STATE_OUT_OF_ORDER;
    default:
        break;
    }

    return RESPONSE_LANE_STATE_UNKNOWN;
}

static int64_t out_of_order(alarm_id_t id, void *user_data)
{
    (void)id;

    lane_group_t* group = (lane_group_t*)user_data;

    group->out_of_order = true;

    set_lane_out_of_order(group->left_lane);
    set_lane_out_of_order(group->center_lane);
    set_lane_out_of_order(group->right_lane);
}

static void reset_out_of_order_alarm(lane_group_t* group)
{
    cancel_alarm(group->out_of_order_alarm_id);
    group->out_of_order_alarm_id = add_alarm_in_ms(
        OUT_OF_ORDER_TIMEOUT_MS,
        &out_of_order,
        group,
        false);
}

static response_t make_lane_state_response(lane_group_t* group)
{
    response_t response = { .id = RESPONSE_ID_LANE_STATES };

    response.lane_states.left_lane_state = get_lane_state(group->left_lane);
    response.lane_states.center_lane_state = get_lane_state(group->center_lane);
    response.lane_states.right_lane_state = get_lane_state(group->right_lane);

    response.lane_states.left_lane_cars_on_lane =
        lane_get_cars_on_lane(group->left_lane);
    response.lane_states.center_lane_cars_on_lane =
        lane_get_cars_on_lane(group->center_lane);
    response.lane_states.right_lane_cars_on_lane =
        lane_get_cars_on_lane(group->right_lane);

    return response;
}

void lane_group_init(lane_group_t* group,
                     const lane_group_config_t* config)
{
    group->left_lane = config->left_lane;
    group->center_lane = config->center_lane;
    group->right_lane = config->right_lane;

    group->out_of_order = false;
    group->out_of_order_alarm_id =
        add_alarm_in_ms(OUT_OF_ORDER_TIMEOUT_MS, &out_of_order, group, false);
}

static response_t handle_state_request(lane_group_t* group, request_state_command_t command)
{
    bool success = request_lane_state(group->left_lane,
                                      command.left_lane_state);

    success = request_lane_state(group->center_lane,
                                 command.center_lane_state) && success;

    success = request_lane_state(group->right_lane,
                                 command.right_lane_state) && success;

    if (success)
    {
        reset_out_of_order_alarm(group);
    }

    return make_lane_state_response(group);
}

response_t handle_debug_command(lane_group_t* group, debug_command_t command)
{
    response_t response = { .id = RESPONSE_ID_DEBUG };

    reset_out_of_order_alarm(group);

    lane_set_debug_command(group->right_lane,
                           command.right_lane_red_on,
                           command.right_lane_yellow_on,
                           command.right_lane_green_on);

    lane_set_debug_command(group->center_lane,
                           command.center_lane_red_on,
                           command.center_lane_yellow_on,
                           command.center_lane_green_on);

    lane_set_debug_command(group->left_lane,
                           command.left_lane_red_on,
                           command.left_lane_yellow_on,
                           command.left_lane_green_on);

    return response;
}

response_t lane_group_handle_command(lane_group_t* group,
                                     command_t command)
{
    response_t response = { .id = RESPONSE_ID_UNKNOWN };

    if (command.id == COMMAND_ID_REQUEST_STATE)
    {
        response = handle_state_request(group, command.request_state);
    }
    else if (command.id == COMMAND_ID_DEBUG)
    {
        response = handle_debug_command(group, command.debug);
    }

    return response;
}
