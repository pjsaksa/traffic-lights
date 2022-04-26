#include "three_light_lane.h"

#include <pico/stdlib.h>

#include <stdio.h>

#define OUT_OF_ORDER_BLINK_INTERVAL_MS  (1500)
#define YELLOW_LIGHT_TRANSITION_DELAY_MS    (2000)

static void control_light(uint light_gpio, bool turn_on)
{
    if (light_gpio > 0)
    {
        gpio_put(light_gpio, turn_on);
    }
}

static void turn_lights_off(three_light_lane_t* lane)
{
    control_light(lane->red_light, false);
    control_light(lane->yellow_light, false);
    control_light(lane->green_light, false);
}

static bool blink_yellow_light(struct repeating_timer* timer)
{
    three_light_lane_t* tllane = (three_light_lane_t*)timer->user_data;
    control_light(tllane->yellow_light, !gpio_get(tllane->yellow_light));
    return true;
}

static void start_out_of_order_blink(three_light_lane_t* lane)
{
    turn_lights_off(lane);
    add_repeating_timer_ms(OUT_OF_ORDER_BLINK_INTERVAL_MS,
                           &blink_yellow_light,
                           lane,
                           &lane->out_of_order_blink_timer);
}

static void stop_out_of_order_blink(three_light_lane_t* lane)
{
    cancel_repeating_timer(&lane->out_of_order_blink_timer);
}

static int64_t state_transition_completed(alarm_id_t id, void* user_data)
{
    (void)id;
    three_light_lane_t* tllane = (three_light_lane_t*)user_data;

    tllane->state_transition_alarm = -1;

    if (tllane->state == LANE_STATE_TRANSITION_TO_STOP)
    {
        control_light(tllane->red_light, true);
        control_light(tllane->yellow_light, false);
        control_light(tllane->green_light, false);

        tllane->state = LANE_STATE_STOP;
    }
    else if (tllane->state == LANE_STATE_TRANSITION_TO_GO)
    {
        control_light(tllane->red_light, false);
        control_light(tllane->yellow_light, false);
        control_light(tllane->green_light, true);

        tllane->state = LANE_STATE_GO;
    }

    return 0;
}

static void start_transition_to_stop(three_light_lane_t* lane)
{
    if (lane->state_transition_alarm > 0)
    {
        cancel_alarm(lane->state_transition_alarm);
    }

    control_light(lane->red_light, false);
    control_light(lane->yellow_light, true);
    control_light(lane->green_light, false);

    lane->state_transition_alarm = add_alarm_in_ms(
        YELLOW_LIGHT_TRANSITION_DELAY_MS,
        &state_transition_completed,
        lane,
        true);
}

static void start_transition_to_go(three_light_lane_t* lane)
{
    if (lane->state_transition_alarm > 0)
    {
        cancel_alarm(lane->state_transition_alarm);
    }

    control_light(lane->red_light, true);
    control_light(lane->yellow_light, true);
    control_light(lane->green_light, false);

    lane->state_transition_alarm = add_alarm_in_ms(
        YELLOW_LIGHT_TRANSITION_DELAY_MS,
        &state_transition_completed,
        lane,
        true);
}

static void request_lane_state(lane_t* lane, lane_state_req_t state_req)
{
    three_light_lane_t* tllane = (three_light_lane_t*)lane;

    if (state_req != LANE_STATE_REQ_OUT_OF_ORDER
        && tllane->state == LANE_STATE_OUT_OF_ORDER)
    {
        stop_out_of_order_blink(tllane);
    }

    switch (state_req)
    {
    case LANE_STATE_REQ_NONE:
        turn_lights_off(tllane);
        tllane->state = LANE_STATE_OFF;
        break;
    case LANE_STATE_REQ_STOP:
        start_transition_to_stop(tllane);
        tllane->state = LANE_STATE_TRANSITION_TO_STOP;
        break;
    case LANE_STATE_REQ_GO:
        start_transition_to_go(tllane);
        tllane->state = LANE_STATE_TRANSITION_TO_GO;
        break;
    case LANE_STATE_REQ_OUT_OF_ORDER:
        start_out_of_order_blink(tllane);
        tllane->state = LANE_STATE_OUT_OF_ORDER;
        break;
    default:
        break;
    }
}

static lane_state_t get_lane_state(lane_t* lane)
{
    three_light_lane_t* tllane = (three_light_lane_t*)lane;

    return tllane->state;
}

static void init_light_gpio(uint gpio)
{
    if (gpio > 0)
    {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
    }
}

static void init_light_gpios(three_light_lane_t* lane)
{
    init_light_gpio(lane->red_light);
    init_light_gpio(lane->yellow_light);
    init_light_gpio(lane->green_light);
}

lane_t* three_light_lane_init(three_light_lane_t* three_light_lane,
                              uint red_light,
                              uint yellow_light,
                              uint green_light)
{
    three_light_lane->base.virtual_funcs.request_lane_state =
        &request_lane_state;
    three_light_lane->base.virtual_funcs.get_lane_state = &get_lane_state;

    three_light_lane->state = LANE_STATE_OFF;

    three_light_lane->red_light = red_light;
    three_light_lane->yellow_light = yellow_light;
    three_light_lane->green_light = green_light;

    three_light_lane->state_transition_alarm = -1;

    init_light_gpios(three_light_lane);

    return (lane_t*)three_light_lane;
}
