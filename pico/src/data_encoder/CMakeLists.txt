add_library(data_encoder data_encoder.c)
target_include_directories(data_encoder PUBLIC include)
target_link_libraries(data_encoder messages pico_stdlib)
