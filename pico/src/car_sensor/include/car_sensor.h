#pragma once

#include <pico/stdlib.h>
#include <stdbool.h>

typedef struct {
    uint gpio;
    bool cars_detected;
} car_sensor_t;

void car_sensor_init(car_sensor_t* sensor, uint gpio);

bool car_sensor_cars_detected(car_sensor_t* sensor);

void car_sensor_clear(car_sensor_t* sensor);