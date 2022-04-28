#pragma once

#include "messages.h"

#include <hardware/spi.h>
#include <stdint.h>

typedef struct {
    spi_inst_t* spi;
    uint8_t rx_buffer[sizeof(raw_command_t)];
    size_t rx_count;
} data_comm_t;

void data_comm_init(data_comm_t* data_comm,
                    spi_inst_t* spi,
                    uint spi_rx,
                    uint spi_tx,
                    uint spi_clk,
                    uint spi_cs);

raw_command_t data_comm_exchange_raw_data(data_comm_t* data_comm, uint32_t tx_data);
