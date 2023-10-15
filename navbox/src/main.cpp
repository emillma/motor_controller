#include <regex>
#include <string>

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"

#include "interface_usb.hpp"
#include "leds.hpp"
#include "inverter_pio.hpp"
#include "trigger_pio.hpp"
#include "stim_uart.hpp"
#include "f9p_i2c.hpp"
#include "uart_pio.hpp"

#define queue_size 8
queue_t ab_queue;
queue_t ba_queue;
f9p_message_t f9p_slots[queue_size];

stim_message_t stim_buffers[2];

int main()
{
    set_sys_clock_khz(133000, true);
    stdio_init_all();

    init_led();

    for (int i = 0; i < queue_size; i++)
        queue_add_blocking(&ab_queue, &f9p_slots[i]);

    usb_init();
    init_trigger_pio();
    init_inverter_pio();
    // stim_init();
    // pwm_init();
    i2c_init();

    f9p_message_t msg;
    multicore_launch_core1(core1_entry);
    trigger_start();
    while (true)
    {
        watchdog_update();
        stim_forward();
        if (queue_try_remove(&ba_queue, &msg))
        {
            usb_send_id(msg.id);
            usb_send_stuffed(msg.data, msg.size);
            queue_add_blocking(&ab_queue, &msg);
        }
    }
}
