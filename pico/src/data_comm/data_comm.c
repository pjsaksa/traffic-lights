#include "data_comm.h"
#include "clock.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define SPI_BAUDRATE    (500000)
#define SPI_DATABITS    (8)
#define SPI_EXCHANGE_TIMEOUT_MS (500)

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

static bool exchange_timeout(uint32_t start_ms, uint32_t timeout_ms)
{
    return clock_get_ms() - start_ms > timeout_ms;
}

static void flush_spi(data_comm_t* data_comm)
{
    const uint8_t tx_data = 0;
    uint8_t rx_data;

    while (spi_is_readable(data_comm->spi))
    {
        spi_read_blocking(data_comm->spi, tx_data, &rx_data, sizeof(rx_data));
    }
}

bool data_comm_exchange_raw_data(data_comm_t* data_comm,
                                 raw_command_t* raw_command,
                                 const raw_response_t* raw_response)
{
//    uint8_t byte_count = 0;
//    uint32_t start_ms = clock_get_ms();
//
//    uint8_t* rx_buf = (uint8_t*)raw_command;
//    const uint8_t* tx_buf = (const uint8_t*)raw_response;
//
//    if (spi_is_readable(data_comm->spi))
//    {
//        while (byte_count < sizeof(raw_command_t)
//            && !exchange_timeout(start_ms, SPI_EXCHANGE_TIMEOUT_MS))
//        {
//            if (spi_is_readable(data_comm->spi))
//            {
//                spi_write_read_blocking(data_comm->spi,
//                                        (const uint8_t*)&tx_buf[byte_count],
//                                        (uint8_t*)&rx_buf[byte_count],
//                                        sizeof(uint8_t));
//                ++byte_count;
//            }
//        }
//
//        printf("count: %d\n", byte_count);
//    }
//
//    flush_spi(data_comm);
//
//    return byte_count == sizeof(raw_command_t);

    bool command_received = false;

    if (spi_is_readable(data_comm->spi))
    {
        spi_write_read_blocking(data_comm->spi,
                                (const uint8_t*) raw_response,
                                (uint8_t*) raw_command,
                                sizeof(raw_command_t));

        command_received = true;
    }

//    flush_spi(data_comm);

    return command_received;
}
