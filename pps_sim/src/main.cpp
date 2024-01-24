#include "pico/stdlib.h"

// #include "interface_usb.hpp"

int main()
{
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_init(26);
    gpio_set_dir(26, GPIO_OUT);
    while (true)
    {
        gpio_put(25, 1);
        gpio_put(26, 1);
        sleep_ms(950);
        gpio_put(25, 0);
        gpio_put(26, 0);
        sleep_ms(150);
    }
}