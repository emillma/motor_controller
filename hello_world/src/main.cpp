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
#include "trigger_pwm.hpp"
#include "stim_uart.hpp"
#include "f9p_i2c.hpp"

#define queue_size 8
queue_t ab_queue;
queue_t ba_queue;
f9p_message_t f9p_slots[queue_size];

__attribute__((aligned(32))) static uint8_t stim_buffer[2][STIM_BUFFER_SIZE];

void core1_entry()
{
    f9p_message_t msg;
    while (true)
    {
        queue_remove_blocking(&ab_queue, &msg);
        if (f9p_a_forward(&msg))

            queue_add_blocking(&ba_queue, &msg);
        else
            queue_add_blocking(&ab_queue, &msg);
        queue_remove_blocking(&ab_queue, &msg);
        if (f9p_b_forward(&msg))

            queue_add_blocking(&ba_queue, &msg);
        else
            queue_add_blocking(&ab_queue, &msg);
    }
}

int main()
{
    watchdog_enable(5000, 1);
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    queue_init(&ab_queue, sizeof(f9p_message_t), queue_size);
    queue_init(&ba_queue, sizeof(f9p_message_t), queue_size);

    for (int i = 0; i < queue_size; i++)
        queue_add_blocking(&ab_queue, &f9p_slots[i]);

    usb_init();
    stim_init(stim_buffer);
    pwm_init();
    pio_inverter_init();
    i2c_init();

    f9p_message_t msg;
    multicore_launch_core1(core1_entry);

    while (true)
    {
        watchdog_update();
        stim_forward(stim_buffer);
        if (queue_try_remove(&ba_queue, &msg))
        {
            usb_send_id(msg.id);
            usb_send_stuffed(msg.data, msg.size);
            queue_add_blocking(&ab_queue, &msg);
        }
    }
}
