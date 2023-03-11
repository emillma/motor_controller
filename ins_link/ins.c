#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"

void interrupt(uint gpio, uint32_t events)
{
    char *str = "Interrupt";
    printf("Interrupt %s \n", events);
}

int main()
{
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    watchdog_enable(5000, 1);

    for (int i = 0; i < 20; i++)
    {
        gpio_put(25, 1);
        sleep_ms(50);
        gpio_put(25, 0);
        sleep_ms(50);
    }

    while (true)
    {
        printf("Hello, world!\n");
        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
        sleep_ms(500);
        watchdog_update();
    }
    return 0;
}
