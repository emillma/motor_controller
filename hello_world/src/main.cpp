#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "leds.hpp"
#include "setup_pio.hpp"
#include "setup_pwm.hpp"
#include "setup_usb.hpp"
#include "setup_uart.hpp"
int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    init_usb();
    init_uart();
    init_pwm();
    init_pio_inverter();

    while (true)
    {
        handle_usb_in();
        sleep_ms(1000);
    }
}