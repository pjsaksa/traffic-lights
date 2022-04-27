#include "traffic_light_group.h"
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
        lane_request_light_state(lane, LANE_STATE_REQ_OUT_OF_ORDER);
    }
}

static bool request_lane_state(lane_t* lane, message_state_req_t new_state)
{
    bool success = true;

    if (lane)
    {
        switch (new_state)
        {
        case MESSAGE_STATE_REQ_STOP:
            lane_request_light_state(lane, LANE_STATE_REQ_STOP);
            break;
        case MESSAGE_STATE_REQ_GO:
            lane_request_light_state(lane, LANE_STATE_REQ_GO);
            break;
        case MESSAGE_STATE_REQ_UNKNOWN:
        default:
            success = false;
            break;
        }
    }

    return success;
}

void traffic_light_group_init(traffic_light_group_t* group,
                              const traffic_light_group_config_t* config)
{
    group->left_lane = config->left_lane;
    group->center_lane = config->center_lane;
    group->right_lane = config->right_lane;

    group->previousCommand_ms = get_ms();
    group->out_of_order = false;
}

void traffic_light_group_execute(traffic_light_group_t* group,
                                 message_t message)
{
    if (message.id == MESSAGE_ID_REQUEST_STATE)
    {
        bool success = request_lane_state(group->left_lane,
                                          message.request_state.left_lane.requested_state);

        success = request_lane_state(group->center_lane,
                                     message.request_state.center_lane.requested_state) && success;

        success = request_lane_state(group->right_lane,
                                     message.request_state.right_lane.requested_state) && success;

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
