#pragma once

#include "lane.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    lane_t* left_lane;
    lane_t* center_lane;
    lane_t* right_lane;
} traffic_light_group_config_t;

typedef struct
{
    lane_t* left_lane;
    lane_t* center_lane;
    lane_t* right_lane;

    uint32_t previousCommand_ms;
    bool out_of_order;
} traffic_light_group_t;

void traffic_light_group_init(traffic_light_group_t* logic,
                              const traffic_light_group_config_t* config);

void traffic_light_group_execute(traffic_light_group_t* logic);
