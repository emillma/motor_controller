#include <regex>
#include <string>

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"

#include "interface_usb.hpp"
#include "leds.hpp"
#include "trigger_pio.hpp"
#include "uart_pio.hpp"
#include "button.hpp"

int main()
{
    set_sys_clock_khz(133000, true);
    stdio_init_all();
    init_led();

    std::array<reader_t, 3> readers = {
        get_reader(9, 1843200, 9),
        get_reader(27, 921600, 27),
        get_reader(26, 921600, 26),
    };

    usb_init();
    init_trigger_pio();
    trigger_start();
    watchdog_enable(10000, true);
    // check if watchdog was triggered

    uint64_t time = time_us_64();
    while (true)
    {
        for (reader_t &reader : readers)
        {
            if (!dma_channel_is_busy(reader.dma_chans[reader.current]))
            {
                blink_for(10);
                watchdog_update();
                fwrite(reader_switch(reader), 1, chunk_size, stdout);

                // led_toggle();
            }
        }

        if (time_us_64() - time > 10000)
        {
            time = time_us_64();
            if (get_bootsel_button())
            {
                while (true)
                {
                    watchdog_update();
                    led_on();
                    sleep_ms(400);
                    led_off();
                    sleep_ms(400);
                }
            }
        }
    }
}