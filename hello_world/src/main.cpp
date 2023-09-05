#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "leds.hpp"
#include "pio_setup.hpp"
#include "pwm_setup.hpp"
#include "stim_setup.hpp"
#include "usb_setup.hpp"

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    usb_init();
    stim_init();
    pwm_init();
    init_pio_inverter();

    while (true)
    {
        stim_handle_input();
        usb_handle_input();
        // sleep_ms(1);
    }
}