#include "three_light_lane.h"

#include <pico/stdlib.h>

#include <stdio.h>

#define OUT_OF_ORDER_BLINK_INTERVAL_MS  (1500)

static void turn_lights_off(three_light_lane_t* lane)
{
    gpio_put(lane->red_light, false);
    gpio_put(lane->yellow_light, false);
    gpio_put(lane->green_light, false);
}

static void turn_red_light_on(three_light_lane_t* lane)
{
    gpio_put(lane->red_light, true);
    gpio_put(lane->yellow_light, false);
    gpio_put(lane->green_light, false);
}

static void turn_red_and_yellow_light_on(three_light_lane_t* lane)
{
    gpio_put(lane->red_light, true);
    gpio_put(lane->yellow_light, true);
    gpio_put(lane->green_light, false);
}

static void turn_yellow_light_on(three_light_lane_t* lane)
{
    gpio_put(lane->red_light, false);
    gpio_put(lane->yellow_light, true);
    gpio_put(lane->green_light, false);
}

static void turn_green_light_on(three_light_lane_t* lane)
{
    gpio_put(lane->red_light, false);
    gpio_put(lane->yellow_light, false);
    gpio_put(lane->green_light, true);
}

static bool blink_yellow_light(struct repeating_timer* timer)
{
    three_light_lane_t* tllane = (three_light_lane_t*)timer->user_data;

    printf("Blinking yellow light: %d\n", tllane->yellow_light);

    gpio_put(tllane->yellow_light, !gpio_get(tllane->yellow_light));
    return true;
}

static void start_out_of_order_blink(three_light_lane_t* lane)
{
    printf("Out of order\n");
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

static void request_light_state(lane_t* lane, lane_state_t state)
{
    three_light_lane_t* tllane = (three_light_lane_t*)lane;

    if (state != LANE_STATE_OUT_OF_ORDER
        && tllane->state == LANE_STATE_OUT_OF_ORDER)
    {
        stop_out_of_order_blink(tllane);
    }

    switch (state)
    {
    case LANE_STATE_NONE:
        turn_lights_off(tllane);
        break;
    case LANE_STATE_RED:
        turn_red_light_on(tllane);
        break;
    case LANE_STATE_YELLOW:
        if (tllane->state == LANE_STATE_RED)
        {
            turn_red_and_yellow_light_on(tllane);
        }
        else
        {
            turn_yellow_light_on(tllane);
        }
        break;
    case LANE_STATE_GREEN:
        turn_green_light_on(tllane);
        break;
    case LANE_STATE_OUT_OF_ORDER:
        start_out_of_order_blink(tllane);
        break;
    default:
        break;
    }

    tllane->state = state;
}

static lane_state_t get_light_state(lane_t* lane)
{
    three_light_lane_t* tllane = (three_light_lane_t*)lane;

    return tllane->state;
}

static void init_light_gpio(three_light_lane_t* lane)
{
    gpio_init(lane->red_light);
    gpio_init(lane->yellow_light);
    gpio_init(lane->green_light);

    gpio_set_dir(lane->red_light, GPIO_OUT);
    gpio_set_dir(lane->yellow_light, GPIO_OUT);
    gpio_set_dir(lane->green_light, GPIO_OUT);
}

lane_t* three_light_lane_init(three_light_lane_t* three_light_lane,
                              uint red_light,
                              uint yellow_light,
                              uint green_light)
{
    three_light_lane->base.virtual_funcs.request_light_state = &request_light_state;
    three_light_lane->base.virtual_funcs.get_light_state = &get_light_state;

    three_light_lane->state = LANE_STATE_NONE;

    three_light_lane->red_light = red_light;
    three_light_lane->yellow_light = yellow_light;
    three_light_lane->green_light = green_light;

    init_light_gpio(three_light_lane);

    return (lane_t*)three_light_lane;
}
