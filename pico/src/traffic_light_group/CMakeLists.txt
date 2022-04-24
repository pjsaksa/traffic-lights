add_library(traffic_light_group traffic_light_group.c)

target_include_directories(traffic_light_group PUBLIC include)
target_link_libraries(traffic_light_group
    PUBLIC lane_interface
    PRIVATE pico_stdlib)
