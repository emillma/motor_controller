#include <regex>
#include <string>

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/watchdog.h"

#include "interface_usb.hpp"
#include "leds.hpp"
#include "inverter_pio.hpp"
#include "trigger_pwm.hpp"
#include "stim_uart.hpp"
#include "f9p_i2c.hpp"

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    __attribute__((aligned(32))) static uint8_t stim_buffer[2][STIM_BUFFER_SIZE];

    size_t stim_buffer_size = sizeof(stim_buffer);

    usb_init();
    stim_init(stim_buffer);
    pwm_init();
    pio_inverter_init();
    i2c_init();

    watchdog_enable(5000, 1);
    while (true)
    {
        watchdog_update();
        stim_forward(stim_buffer);
        f9p_a_forward();
        f9p_b_forward();
        // sleep_ms(500);
    }
}
