cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(traffic_lights C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()


add_executable(traffic_lights main.c)

# pull in common dependencies
target_link_libraries(traffic_lights pico_stdlib)

## create map/bin/hex file etc.
pico_add_extra_outputs(traffic_lights)
