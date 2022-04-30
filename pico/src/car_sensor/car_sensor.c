#include "car_sensor.h"

#include <stdio.h>

#define UNDEFINED_GPIO (0xFFFF)
#define MAX_SENSOR_COUNT (3)

typedef struct {
    uint gpio;
    car_sensor_t* sensor;
} car_sensor_gpio_wrapper_t;

static car_sensor_gpio_wrapper_t sensors[MAX_SENSOR_COUNT] = {
    { .gpio = UNDEFINED_GPIO, .sensor = 0 },
    { .gpio = UNDEFINED_GPIO, .sensor = 0 },
    { .gpio = UNDEFINED_GPIO, .sensor = 0 },
};

static void gpio_irq_callback(uint gpio, uint32_t events);

static void register_sensor_wrapper(uint gpio, car_sensor_t* sensor)
{
    for (uint i = 0; i < MAX_SENSOR_COUNT; ++i)
    {
        if (sensors[i].gpio == UNDEFINED_GPIO)
        {
            sensors[i].gpio = gpio;
            sensors[i].sensor = sensor;
            break;
        }
    }
}

void car_sensor_init(car_sensor_t* sensor, uint gpio)
{
    sensor->gpio = gpio;
    sensor->cars_detected = false;

    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_set_pulls(gpio, false, true);

    register_sensor_wrapper(gpio, sensor);
    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE, true, &gpio_irq_callback);
}

bool car_sensor_cars_detected(car_sensor_t* sensor)
{
    return sensor->cars_detected;
}

void car_sensor_clear(car_sensor_t* sensor)
{
    sensor->cars_detected = false;
}

static void gpio_irq_callback(uint gpio, uint32_t events)
{
    for (uint i = 0; i < MAX_SENSOR_COUNT; ++i)
    {
        if ((sensors[i].gpio == gpio) && (events & GPIO_IRQ_EDGE_RISE))
        {
            sensors[i].sensor->cars_detected = true;
            printf("Car detected: %d\n", gpio);
        }
    }
}
