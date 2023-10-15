#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"

#include "leds.hpp"
#include "f9p_i2c.hpp"
#include "log_uart.hpp"
#include "interface_usb.hpp"
#include "trigger_pio.hpp"

#define queue_size 4
queue_t ab_queue;
queue_t ba_queue;
f9p_message_t f9p_slots[queue_size];

void core1_entry()
{
    f9p_message_t msg;
    while (true)
    {
        for (uint8_t i = 30; i < 32; i++)
        {
            queue_remove_blocking(&ab_queue, &msg);
            if (f9p_forward(i, &msg))
                queue_add_blocking(&ba_queue, &msg);
            else
                queue_add_blocking(&ab_queue, &msg);
        }
    }
}

int main()
{

    stdio_init_all();
    gpio_init(0);
    gpio_init(28);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_dir(28, GPIO_OUT);
    gpio_put(0, 1);
    gpio_put(28, 0);
    sleep_ms(100);
    gpio_put(28, 1);
    sleep_ms(5000);
    // set_sys_clock_khz(133000, true);

    const auto pin_stamp = 11;
    gpio_init(pin_stamp);
    gpio_set_dir(pin_stamp, GPIO_IN);
    gpio_put(pin_stamp, 1);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();
    trigger_pio_init();
    queue_init(&ab_queue, sizeof(f9p_message_t), queue_size);
    queue_init(&ba_queue, sizeof(f9p_message_t), queue_size);

    for (int i = 0; i < queue_size; i++)
        queue_add_blocking(&ab_queue, &f9p_slots[i]);

    usb_init();
    logger_init();
    i2c_init();

    f9p_message_t msg;
    uint32_t groups = 0;
    multicore_launch_core1(core1_entry);

    uint64_t stamptime = 0;
    uint32_t stamp_count = 0;

    watchdog_enable(1000, true);
    trigger_start();
    uint32_t trigger_count = 0;
    for (int i = 0;; i++)
    {

        if (queue_try_remove(&ba_queue, &msg))
        {
            watchdog_update();
            blink_for(10);
            uart_send_id(msg.id);
            uart_send_stuffed(msg.data, msg.size);
            queue_add_blocking(&ab_queue, &msg);
            sleep_us(200);
        }
        if (trigger_pio_get())
        {
            uart_send_id(0x80);
            uart_send_stuffed((uint8_t *)&trigger_count, 4);
            trigger_count++;
        }
    }
}
