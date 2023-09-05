#pragma once
#include "hardware/uart.h"

#define stim_uart_id uart0

void stim_init()
{
    int baud_rate = 921600;
    uart_init(uart0, baud_rate);
    // set pin 0 and 1 to uart

    // gpio_set_pulls(0, false, false);

    gpio_set_function(0, GPIO_FUNC_UART);
    // gpio_set_input_hysteresis_enabled(0, false);
    gpio_set_function(1, GPIO_FUNC_UART);
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