#pragma once

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

uint32_t data_comm_exchange_raw_data(data_comm_t* data_comm, uint32_t tx_data);
