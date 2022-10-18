#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "pio/encoder.h"
#include "hardware/clocks.h"

int main()
{
    // Set up our UART with the required speed.
    stdio_init_all();
    encoder_program_init();
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    uint64_t t0 = get_absolute_time();

    while (true)
    {
        uint32_t val = encoder_read_blocking();
        uint32_t val2 = get_absolute_time() - t0;
        if (val % 100 == 0)
            printf("%d\n", val);
        // sleep_ms(1000);
    }
}