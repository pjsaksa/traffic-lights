#pragma once

#include "lane.h"
#include "messages.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    lane_t* left_lane;
    lane_t* center_lane;
    lane_t* right_lane;
} lane_group_config_t;

typedef struct
{
    lane_t* left_lane;
    lane_t* center_lane;
    lane_t* right_lane;

    uint32_t previousCommand_ms;
    bool out_of_order;
} lane_group_t;

void lane_group_init(lane_group_t* logic,
                     const lane_group_config_t* config);

void lane_group_execute(lane_group_t* group,
                        command_t command);
