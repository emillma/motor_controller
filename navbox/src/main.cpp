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
#include "queues.hpp"

int main()
{
    // set_sys_clock_khz(133000, true);
    stdio_init_all();

    init_led();

    queue_init(&free_queue, sizeof(recording_t), queue_size);
    queue_init(&full_queue, sizeof(recording_t), queue_size);
    for (int i = 0; i < queue_size; i++)
        queue_add_blocking(&free_queue, &recordings[i]);

    reader_t reader = get_reader(9, 1843200, 9);

    usb_init();
    init_trigger_pio();

    trigger_start();
    char c;
    // led_on();
    while (true)
    {
        if (dma_channel_is_busy(reader.dma_chans[reader.current]))
        {
            int ready = reader.current;
            reader_switch(&reader);
            // usb_send_byte(read(reader.sm));
            usb_send_id(reader.id);
            usb_send_stuffed(reader.data[ready], chunk_size);
            // usb_flush();
            led_on();
        }
        else
        {
            // led_off();
            // usb_send_byte(0);
            // usb_flush();
        }
        // usb_send_byte(read(reader.sm));
    }
}
