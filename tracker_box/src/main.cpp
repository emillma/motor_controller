#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"

#include "leds.hpp"
#include "trigger_pio.hpp"
#include "f9p_i2c.hpp"
#include "log_uart.hpp"
#include "interface_usb.hpp"

#define queue_size 8
queue_t ab_queue;
queue_t ba_queue;
f9p_message_t f9p_slots[queue_size];

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

    stdio_init_all();
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    gpio_put(0, 1);
    sleep_ms(5000);

    // set_sys_clock_khz(133000, true);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    queue_init(&ab_queue, sizeof(f9p_message_t), queue_size);
    queue_init(&ba_queue, sizeof(f9p_message_t), queue_size);

    for (int i = 0; i < queue_size; i++)
        queue_add_blocking(&ab_queue, &f9p_slots[i]);

    trigger_pio_init();
    usb_init();
    logger_init();
    i2c_init();

    f9p_message_t msg;
    multicore_launch_core1(core1_entry);

    trigger_start();

    for (int i = 0;; i++)
    {

        // gpio_put(25, 1);1
        if (queue_try_remove(&ba_queue, &msg))
        {
            blink_for(10);
            uart_send_id(msg.id);
            uart_send_stuffed(msg.data, msg.size);
            queue_add_blocking(&ab_queue, &msg);
        }
        // uart_write_char('a');
        // uart_write_char(10);
        sleep_us(100);
    }
}
