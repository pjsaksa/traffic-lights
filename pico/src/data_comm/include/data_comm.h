#pragma once

#include "messages.h"

#include <hardware/spi.h>
#include <stdint.h>

typedef struct {
    spi_inst_t* spi;
} data_comm_t;

void data_comm_init(data_comm_t* data_comm,
                    spi_inst_t* spi,
                    uint spi_rx,
                    uint spi_tx,
                    uint spi_clk,
                    uint spi_cs);

bool data_comm_exchange_raw_data(data_comm_t* data_comm,
                                 raw_command_t* raw_command,
                                 const raw_response_t* raw_response);
