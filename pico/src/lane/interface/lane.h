#pragma once

typedef struct lane_s lane_t;

typedef enum {
    LANE_STATE_NONE,
    LANE_STATE_RED,
    LANE_STATE_YELLOW,
    LANE_STATE_GREEN,
    LANE_STATE_OUT_OF_ORDER
} lane_state_t;

typedef struct lane_virtual_funcs_s {
    void (*request_light_state)(lane_t* lane, lane_state_t state);
    lane_state_t (*get_light_state)(lane_t* lane);
} lane_virtual_funcs_t;

typedef struct {
    lane_virtual_funcs_t virtual_funcs;
} lane_base_t;

static inline void lane_request_light_state(lane_t* lane, lane_state_t state)
{
    lane_base_t* base = (lane_base_t*)lane;
    base->virtual_funcs.request_light_state(lane, state);
}

static inline lane_state_t lane_get_light_state(lane_t* lane)
{
    lane_base_t* base = (lane_base_t*)lane;
    return base->virtual_funcs.get_light_state(lane);
}
