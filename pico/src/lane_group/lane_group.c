#include "lane_group.h"
#include "lane.h"
#include "messages.h"

#include <pico/stdlib.h>

#define OUT_OF_ORDER_TIMEOUT_MS     (10000)

static inline uint32_t get_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}

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

void lane_group_init(lane_group_t* logic,
                     const lane_group_config_t* config)
{
    logic->left_lane = config->left_lane;
    logic->center_lane = config->center_lane;
    logic->right_lane = config->right_lane;

    logic->previousCommand_ms = get_ms();
    logic->out_of_order = false;
}

void lane_group_execute(lane_group_t* group,
                        command_t command)
{
    if (command.id == COMMAND_ID_REQUEST_STATE)
    {
        bool success = request_lane_state(group->left_lane,
                                          command.request_state.left_lane_state);

        success = request_lane_state(group->center_lane,
                                     command.request_state.center_lane_state) && success;

        success = request_lane_state(group->right_lane,
                                     command.request_state.right_lane_state) && success;

        if (success)
        {
            group->previousCommand_ms = get_ms();
        }
    }

    if (!group->out_of_order)
    {
        if (get_ms() - group->previousCommand_ms > OUT_OF_ORDER_TIMEOUT_MS)
        {
            group->out_of_order = true;

            set_lane_out_of_order(group->left_lane);
            set_lane_out_of_order(group->center_lane);
            set_lane_out_of_order(group->right_lane);
        }
    }
}
