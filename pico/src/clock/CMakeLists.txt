add_library(clock INTERFACE)
target_include_directories(clock INTERFACE include)
target_link_libraries(clock INTERFACE pico_stdlib)
