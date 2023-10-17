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

int main()
{
    // set_sys_clock_khz(133000, true);
    stdio_init_all();
    init_led();

    reader_t readers[] = {
        get_reader(0, 1843200, 11),
        get_reader(9, 1843200, 12),
    };

    usb_init();
    init_trigger_pio();
    trigger_start();
    watchdog_enable(1000, true);
    reader_t *reader; // reader_t reader = readers[0];
    while (true)
    {
        for (int i = 0; i < 2; i++)
        {
            reader = &readers[i];
            if (dma_channel_is_busy(reader->dma_chans[reader->current]))
            {
                watchdog_update();
                int ready = reader->current;
                reader_switch(reader);
                fwrite(reader->data[ready], 1, chunk_size, stdout);
                blink_for(10);
            }
        }
    }
}
