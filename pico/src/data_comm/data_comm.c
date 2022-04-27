#include "data_comm.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define SPI_BAUDRATE    (500000)
#define SPI_DATABITS    (8)

void data_comm_init(data_comm_t* data_comm,
                    spi_inst_t* spi,
                    uint spi_rx,
                    uint spi_tx,
                    uint spi_clk,
                    uint spi_cs)
{
    data_comm->spi = spi;

    gpio_set_function(spi_rx, GPIO_FUNC_SPI);
    gpio_set_function(spi_tx, GPIO_FUNC_SPI);
    gpio_set_function(spi_clk, GPIO_FUNC_SPI);
    gpio_set_function(spi_cs, GPIO_FUNC_SPI);

    spi_init(spi, SPI_BAUDRATE);
    spi_set_slave(spi, true);
    spi_set_format(spi, SPI_DATABITS, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

uint32_t data_comm_exchange_raw_data(data_comm_t* data_comm, uint32_t tx_data)
{
    uint32_t raw_data = 0;

    if (spi_is_readable(data_comm->spi))
    {
        spi_write_read_blocking(data_comm->spi,
                                (const uint8_t*)&tx_data,
                                (uint8_t*)&raw_data,
                                sizeof(raw_data));

        printf("Rx: 0x%lx, Tx: 0x%lx\n", raw_data, tx_data);
    }

    return raw_data;
}
