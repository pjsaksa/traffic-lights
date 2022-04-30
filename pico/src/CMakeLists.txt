cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-traffic_lights C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(ENABLE_TEST_BUILDS "Compile test targets" OFF)

# Initialize the SDK
pico_sdk_init()

add_subdirectory(car_sensor)
add_subdirectory(clock)
add_subdirectory(data_comm)
add_subdirectory(data_decoder)
add_subdirectory(data_encoder)
add_subdirectory(lane)
add_subdirectory(messages)
add_subdirectory(lane_group)

add_executable(traffic_lights main.c)
target_link_libraries(traffic_lights
    pico_stdlib
    hardware_spi
    lane_group
    lane
    data_decoder
    data_encoder
    data_comm
    car_sensor)
pico_add_uf2_output(traffic_lights)
pico_add_hex_output(traffic_lights)
pico_enable_stdio_usb(traffic_lights 1)
pico_enable_stdio_uart(traffic_lights 0)

if (ENABLE_TEST_BUILDS)
add_subdirectory(testing/fake_master)
add_subdirectory(testing/spi)
endif()
