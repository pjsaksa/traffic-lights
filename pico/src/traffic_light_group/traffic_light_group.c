#include "traffic_light_group.h"

#include <pico/stdlib.h>

#define OUT_OF_ORDER_TIMEOUT_MS     (5000)

static inline uint32_t get_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}

static void set_lane_out_of_order(lane_t* lane)
{
    if (lane)
    {
        lane_request_light_state(lane, LANE_STATE_OUT_OF_ORDER);
    }
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

void traffic_light_group_execute(traffic_light_group_t* group)
{
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
