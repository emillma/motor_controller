#pragma once
#include "hardware/uart.h"

void init_uart()
{
    auto uart_id = uart0;
    int baud_rate = 921600;
    uart_init(uart0, baud_rate);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
}