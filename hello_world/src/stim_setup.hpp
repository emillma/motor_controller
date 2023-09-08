#pragma once
#include "hardware/uart.h"

#define stim_uart_id uart1

void stim_init()
{
    // int baud_rate = 1843200;
    int baud_rate = 921600;
    // int baud_rate = 460800;
    uart_init(stim_uart_id, baud_rate);

    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);
    // gpio_set_pulls(9, false, false);
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

void stim_write(uint8_t *data, size_t len)
{
    uart_write_blocking(stim_uart_id, data, len);
}
