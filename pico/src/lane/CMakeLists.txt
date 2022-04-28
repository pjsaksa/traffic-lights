add_library(lane lane.c)
target_link_libraries(lane PUBLIC pico_stdlib)
target_include_directories(lane PUBLIC include)
