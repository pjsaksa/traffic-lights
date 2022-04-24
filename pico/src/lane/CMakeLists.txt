add_library(lane_interface INTERFACE)
target_include_directories(lane_interface INTERFACE interface)

add_library(lane three_light_lane.c)
target_link_libraries(lane PUBLIC lane_interface pico_stdlib)
target_include_directories(lane PUBLIC include)
