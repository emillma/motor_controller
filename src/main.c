#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "pio/encoder.h"

int main()
{
    // Set up our UART with the required speed.
    stdio_init_all();
    encoder_program_init();
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    while (true)
    {
        printf("%x\n", encoder_read_blocking());
        // sleep_ms(1000);
    }
}