add_library(data_decoder data_decoder.c)
target_include_directories(data_decoder PUBLIC include)
target_link_libraries(data_decoder PUBLIC messages)
