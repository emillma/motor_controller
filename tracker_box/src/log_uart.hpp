#pragma once
#include "hardware/uart.h"

#define UART_ID uart0

void logger_init()
{
    // Set up our UART with the required speed.
    uart_init(uart0, 460800);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_pull_up(0);
}

void uart_write_char(uint8_t c)
{
    uart_putc_raw(UART_ID, c);
}

void uart_send_stuffed(const uint8_t *data, size_t len)
{
    const uint8_t marker = 0xfe;

    for (size_t idx = 0; idx < len; idx++)
    {
        {
            uart_write_char(data[idx]);
            if (data[idx] == marker)
            {
                uart_write_char(marker);
            }
        }
    }
}

void uart_send_id(uint8_t id)
{
    uart_write_char(0xfe);
    uart_write_char(id);
}
