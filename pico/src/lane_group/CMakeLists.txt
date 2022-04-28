add_library(lane_group lane_group.c)

target_include_directories(lane_group PUBLIC include)
target_link_libraries(lane_group
    PRIVATE pico_stdlib messages lane)
