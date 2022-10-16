#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"

int main()
{
    // Set up our UART with the required speed.
    stdio_init_all();
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    uint pwm_out_start = 10;
    for (uint pin = pwm_out_start; pin < pwm_out_start + 6; pin += 2)
    {

        gpio_set_function(pin, GPIO_FUNC_PWM);
        gpio_set_function(pin + 1, GPIO_FUNC_PWM);

        uint slice_num = pwm_gpio_to_slice_num(pin);
        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, 1.0f);
        pwm_init(slice_num, &config, false);

        uint level = 125;
        pwm_set_wrap(slice_num, level * 2);
        pwm_set_chan_level(slice_num, PWM_CHAN_A, level - 1);
        pwm_set_chan_level(slice_num, PWM_CHAN_B, level + 1);
        // pwm_set_enabled(slice_num, true);
        pwm_set_output_polarity(slice_num, false, true);
        pwm_set_phase_correct(slice_num, true);
    }
    pwm_set_mask_enabled(0b1111111);

    while (true)
    {
        printf("Emil\n");
        for (uint i = 0; i < 6; i++)
        {
            int x = pwm_gpio_to_slice_num(i + pwm_out_start);
            printf("katt %d\n", x);
        };
        sleep_ms(1000);
    }
}