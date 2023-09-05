#pragma once
#include "hardware/uart.h"

#define stim_uart_id uart1

void stim_init()
{
    int baud_rate = 1843200;
    // int baud_rate = 921600;
    uart_init(stim_uart_id, baud_rate);

    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);
}

void stim_handle_input()
{
    bool any = false;
    while (uart_is_readable(stim_uart_id))
    {
        char c = uart_getc(stim_uart_id);
        fwrite(&c, 1, 1, stdout);
        any = true;
    }
    if (any)
        fflush(stdout);
}

void stim_send_char(char c)
{
    uart_putc(stim_uart_id, c);
}