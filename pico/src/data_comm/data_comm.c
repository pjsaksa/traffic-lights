#include "data_comm.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
    data_comm->rx_count = 0;

    gpio_set_function(spi_rx, GPIO_FUNC_SPI);
    gpio_set_function(spi_tx, GPIO_FUNC_SPI);
    gpio_set_function(spi_clk, GPIO_FUNC_SPI);
    gpio_set_function(spi_cs, GPIO_FUNC_SPI);

    spi_init(spi, SPI_BAUDRATE);
    spi_set_slave(spi, true);
    spi_set_format(spi, SPI_DATABITS, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

raw_command_t data_comm_exchange_raw_data(data_comm_t* data_comm, uint32_t tx_data)
{
    raw_command_t raw_command = {0};

    while (spi_is_readable(data_comm->spi) && data_comm->rx_count < sizeof(raw_command_t))
    {
        spi_write_read_blocking(data_comm->spi,
                                (const uint8_t*)&tx_data,
                                &data_comm->rx_buffer[data_comm->rx_count],
                                sizeof(uint8_t));
        data_comm->rx_count++;
    }

    if (data_comm->rx_count == sizeof(raw_command_t))
    {
        memcpy(&raw_command, data_comm->rx_buffer, sizeof(raw_command_t));
        data_comm->rx_count = 0;

        printf("Raw command - id: %d, cmd: %d, data: %llx\n",
               raw_command.frame_id,
               raw_command.command_id,
               raw_command.data);
    }

    return raw_command;
}
