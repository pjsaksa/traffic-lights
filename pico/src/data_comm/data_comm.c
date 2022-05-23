#include "data_comm.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define SPI_BAUDRATE    (500000)
#define SPI_DATABITS    (8)

#ifdef DEBUG_PRINT
static void printhex(const uint8_t* data, size_t size)
{
    for (unsigned i = 0; i < size; ++i)
    {
        printf("%02x", data[i]);
        if (i + 1 < size)
        {
            printf(":");
        }
    }
    printf("\n");
}
#endif

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
    spi_set_format(spi, SPI_DATABITS, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
}

raw_command_t data_comm_exchange_raw_data(data_comm_t* data_comm,
                                          const raw_response_t raw_response)
{
    raw_command_t raw_command;

    spi_write_read_blocking(data_comm->spi,
                            (const uint8_t*) &raw_response,
                            (uint8_t*) &raw_command,
                            sizeof(raw_command_t));

#ifdef DEBUG_PRINT
    printf("Rx: ");
    printhex((uint8_t*)&raw_command, sizeof(raw_command));
    printf("Tx: ");
    printhex((uint8_t*)&raw_response, sizeof(raw_response));
#endif

    return raw_command;
}
