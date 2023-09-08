#pragma once
#include "hardware/uart.h"

#define stim_uart_id uart1

void stim_init()
{
    // int baud_rate = 1843200;
    int baud_rate = 921600;
    uart_init(stim_uart_id, baud_rate);

    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);
}

void stim_forward()
{
    uint8_t data;
    if (uart_is_readable(stim_uart_id))
    {
        usb_send_id(90);
    }
    while (uart_is_readable(stim_uart_id))
    {
        data = uart_getc(stim_uart_id);
        usb_send_stuffed(&data, 1);
    }
}

void stim_from_usb()
{

    bool any = false;
    for (int input = getchar_timeout_us(0); input != PICO_ERROR_TIMEOUT; input = getchar_timeout_us(0))
    {
        uart_putc(stim_uart_id, (char)input);
    }
}