#include "lane.h"
#include "car_sensor.h"

#include <pico/stdlib.h>

#include <stdio.h>

#define OUT_OF_ORDER_BLINK_INTERVAL_MS  (1500)
#define YELLOW_LIGHT_TRANSITION_DELAY_MS    (2000)

static void control_light(uint light_gpio, bool turn_on)
{
    if (light_gpio != LANE_UNUSED_GPIO)
    {
        gpio_put(light_gpio, turn_on);
    }
}

static void cancel_transition_if_active(lane_t* lane)
{
    if (lane->state_transition_alarm > 0)
    {
        cancel_alarm(lane->state_transition_alarm);
        lane->state_transition_alarm = 0;
    }
}

static void turn_lights_off(lane_t* lane)
{
    control_light(lane->red_light, false);
    control_light(lane->yellow_light, false);
    control_light(lane->green_light, false);

    cancel_transition_if_active(lane);
}

static bool blink_yellow_light(struct repeating_timer* timer)
{
    lane_t* lane = (lane_t*)timer->user_data;
    control_light(lane->yellow_light, !gpio_get(lane->yellow_light));
    return true;
}

static void start_out_of_order_blink(lane_t* lane)
{
    turn_lights_off(lane);
    cancel_transition_if_active(lane);
    lane->out_of_order = add_repeating_timer_ms(OUT_OF_ORDER_BLINK_INTERVAL_MS,
                           &blink_yellow_light,
                           lane,
                           &lane->out_of_order_blink_timer);
}

static void stop_out_of_order_blink(lane_t* lane)
{
    if (lane->out_of_order)
    {
        cancel_repeating_timer(&lane->out_of_order_blink_timer);
        lane->out_of_order = false;
    }
}

static int64_t state_transition_completed(alarm_id_t id, void* user_data)
{
    (void)id;
    lane_t* lane = (lane_t*)user_data;

    lane->state_transition_alarm = 0;

    if (lane->state == LANE_STATE_TRANSITION_TO_STOP)
    {
        control_light(lane->red_light, true);
        control_light(lane->yellow_light, false);
        control_light(lane->green_light, false);

        lane->state = LANE_STATE_STOP;
    }
    else if (lane->state == LANE_STATE_TRANSITION_TO_GO)
    {
        control_light(lane->red_light, false);
        control_light(lane->yellow_light, false);
        control_light(lane->green_light, true);

        car_sensor_clear(lane->sensor);

        lane->state = LANE_STATE_GO;
    }

    return 0;
}

static void start_transition_to_stop(lane_t* lane)
{
    control_light(lane->red_light, false);
    control_light(lane->yellow_light, true);
    control_light(lane->green_light, false);

    car_sensor_clear(lane->sensor);

    lane->state_transition_alarm = add_alarm_in_ms(
        YELLOW_LIGHT_TRANSITION_DELAY_MS,
        &state_transition_completed,
        lane,
        false);
}

static void start_transition_to_go(lane_t* lane)
{
    control_light(lane->red_light, true);
    control_light(lane->yellow_light, true);
    control_light(lane->green_light, false);

    lane->state_transition_alarm = add_alarm_in_ms(
        YELLOW_LIGHT_TRANSITION_DELAY_MS,
        &state_transition_completed,
        lane,
        false);
}

static void init_light_gpio(uint gpio)
{
    if (gpio != LANE_UNUSED_GPIO)
    {
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
    }
}

static void init_light_gpios(lane_t* lane)
{
    init_light_gpio(lane->red_light);
    init_light_gpio(lane->yellow_light);
    init_light_gpio(lane->green_light);
}

void lane_init(lane_t* lane,
               uint red_light,
               uint yellow_light,
               uint green_light,
               car_sensor_t* sensor)
{
    lane->state = LANE_STATE_NONE;
    lane->target_req_state = LANE_STATE_REQ_NONE;

    lane->red_light = red_light;
    lane->yellow_light = yellow_light;
    lane->green_light = green_light;

    lane->sensor = sensor;

    lane->state_transition_alarm = 0;
    lane->out_of_order = false;

    init_light_gpios(lane);
}

void lane_request_state(lane_t* lane, lane_state_req_t state_req)
{
    // Ignore request to state already requested
    if (state_req == lane->target_req_state)
    {
        return;
    }

    printf("Lane to state: %d\n", state_req);

    lane->target_req_state = state_req;

    cancel_transition_if_active(lane);

    if (state_req != LANE_STATE_REQ_OUT_OF_ORDER
        && lane->state == LANE_STATE_OUT_OF_ORDER)
    {
        stop_out_of_order_blink(lane);
    }

    switch (state_req)
    {
    case LANE_STATE_REQ_NONE:
        turn_lights_off(lane);
        lane->state = LANE_STATE_NONE;
        break;
    case LANE_STATE_REQ_STOP:
        start_transition_to_stop(lane);
        lane->state = LANE_STATE_TRANSITION_TO_STOP;
        break;
    case LANE_STATE_REQ_GO:
        start_transition_to_go(lane);
        lane->state = LANE_STATE_TRANSITION_TO_GO;
        break;
    case LANE_STATE_REQ_OUT_OF_ORDER:
        start_out_of_order_blink(lane);
        lane->state = LANE_STATE_OUT_OF_ORDER;
        break;
    default:
        break;
    }
}

lane_state_t lane_get_state(lane_t* lane)
{
    return lane->state;
}

bool lane_get_cars_on_lane(lane_t* lane)
{
    return car_sensor_cars_detected(lane->sensor);
}

void lane_set_debug_command(lane_t* lane, bool red_on, bool yellow_on, bool green_on)
{
    cancel_transition_if_active(lane);
    stop_out_of_order_blink(lane);

    control_light(lane->red_light, red_on);
    control_light(lane->yellow_light, yellow_on);
    control_light(lane->green_light, green_on);
}
