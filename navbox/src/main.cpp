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
    init_led();

    std::array<reader_t, 3> readers = {
        get_reader(9, 1843200, 9),
        get_reader(27, 921600, 27),
        get_reader(26, 921600, 26),
    };

    // usb_init();
    init_trigger_pio();
    trigger_start();
    watchdog_enable(10000, true);
    // check if watchdog was triggered
    uint8_t sep[] = {0xde, 0xad, 0xbe, 0xef};
    uint64_t time = time_us_64();
    uint8_t data[chunk_size];
    // fill data with 0xff
    uint8_t *ptr = data;
    memset(data, 0xff, chunk_size);
    while (true)
    {
        for (reader_t &reader : readers)
        {
            if (!dma_channel_is_busy(reader.dma_chans[reader.current]))
            {
                blink_for(50);
                watchdog_update();
                ptr = reader_switch(reader);
                fwrite(ptr, 1, chunk_size, stdout);
            }
        }
    }
}