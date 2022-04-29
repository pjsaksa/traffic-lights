add_library(data_comm data_comm.c)
target_include_directories(data_comm PUBLIC include)
target_link_libraries(data_comm PRIVATE pico_stdlib hardware_spi messages clock)
