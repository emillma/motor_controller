#include <regex>
#include <string>

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"

// #include "interface_usb.hpp"
#include "leds.hpp"
#include "trigger_pio.hpp"
#include "uart_pio.hpp"

int main()
{
    set_sys_clock_khz(133000, true);
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);
    // init_led();

    std::array<reader_t, 3> readers = {
        get_reader(9, 1843200, 9),
        get_reader(27, 921600, 27),
        get_reader(26, 921600, 26),
    };

    init_trigger_pio();
    add_alarm_in_ms(0, trigger_start, NULL, true);

    while (true)
    {
        for (reader_t &reader : readers)
        {
            if (!dma_channel_is_busy(reader.dma_chans[reader.current]))
            {
                // blink_for(10);
                fwrite(reader_switch(reader), 1, chunk_size, stdout);
            }
        }
    }
}