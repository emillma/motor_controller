#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"

#include "leds.hpp"
#include "../pio/pio_setup.h"

int main()
{
    stdio_usb_init();
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);

    // led blink on startup
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    // uart init
    auto uart_id = uart0;
    int baud_rate = 921600;
    uart_init(uart0, baud_rate);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // pwm
    uint slice_num = pwm_gpio_to_slice_num(2);

    gpio_set_function(2, GPIO_FUNC_PWM);
    gpio_set_function(3, GPIO_FUNC_PWM);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_output_polarity(&config, true, true);
    pwm_config_set_phase_correct(&config, true);
    pwm_init(slice_num, &config, false);

    pwm_set_wrap(slice_num, 50000);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 500);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 500);
    pwm_set_enabled(slice_num, true);

    // pio
    invert_program_init();

    // echo
    char c;
    while (true)
    {

        // printf("Line input:");
        for (uint i = 0; i < 265; i++)
        {
            c = uart_getc(uart0);
            fwrite(&c, 1, 1, stdout);
        }
        fflush(stdout);
        // do
        // {
        //     // c = fgetc(stdin);
        //     c = uart_getc(uart0);
        // } while (c != '\n');
    }
}