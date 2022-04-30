add_library(car_sensor car_sensor.c)
target_include_directories(car_sensor PUBLIC include)
target_link_libraries(car_sensor PUBLIC pico_stdlib)
