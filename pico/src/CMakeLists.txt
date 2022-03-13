cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(traffic_lights C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(traffic_lights main.c)
target_link_libraries(traffic_lights pico_stdlib)
pico_add_uf2_output(traffic_lights)

add_subdirectory(testing/spi)
