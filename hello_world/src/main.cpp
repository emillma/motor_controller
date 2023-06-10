#include <unistd.h>
#include <tusb.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "leds.hpp"
int main()
{
    // stdio_usb_init();
    // gpio_init(25);
    // gpio_set_dir(25, GPIO_OUT);
    blink_fast();
    set_sys_clock_khz(250000, false);
    stdio_set_translate_crlf(&stdio_usb, false);
    stdio_init_all();
    uint8_t data[32];
    while (true)
    {
        fwrite(data, 32, 1, stdout);
        // fflush(stdout);
        // sleep_ms(1);
    }
    // stdio_flush();
}