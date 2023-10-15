#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "interface_usb.hpp"
#include "leds.hpp"
#include "inverter_pio.hpp"

int main()
{
    set_sys_clock_khz(133000, true);
    stdio_init_all();

    init_led();

    uart_init(uart1, 1843200);
    gpio_set_function(8, GPIO_FUNC_UART);
    gpio_set_function(9, GPIO_FUNC_UART);

    usb_init();
    init_inverter_pio();

    while (true)
    {
        while (uart_is_readable(uart1))
        {
            usb_send_byte(uart_getc(uart1));
        }
        usb_flush();
        while (true)
        {
            int c = getchar_timeout_us(0);
            if (c == PICO_ERROR_TIMEOUT)
                break;
            uart_putc(uart1, (char)c);
        }
    }
}
