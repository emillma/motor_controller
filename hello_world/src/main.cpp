#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "usb_setup.hpp"
#include "leds.hpp"
#include "pio_setup.hpp"
#include "pwm_setup.hpp"
#include "stim_setup.hpp"
#include "i2c_setup.hpp"

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    usb_init();
    stim_init();
    pwm_init();
    pio_inverter_init();
    i2c_init();

    while (true)
    {
        stim_forward();
        // usb_handle_input();
        // f9p_a_forward();
        // f9p_b_forward();
    }
}
