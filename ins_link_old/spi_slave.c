#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"

#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"

#include "trigger.pio.h"
#include "selecter.pio.h"

#include "core1.h"
#include "header.h"

uint64_t first_trigger_time = 0;
uint64_t last_pps_time = 0;

queue_t message_queue;
queue_t free_queue;

void interrupt(uint gpio, uint32_t events)
{
    absolute_time_t abs_time = get_absolute_time();
    if (gpio == PPS_PIN && first_trigger_time != 0)
    {
        last_pps_time = to_us_since_boot(abs_time) - first_trigger_time - TIME_CORRECTION;
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    pio_interrupt_clear(pio0, 0);
    absolute_time_t abs_time = get_absolute_time();
    first_trigger_time = to_us_since_boot(abs_time);
    return 0;
}

int main()
{
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // init queues
    queue_init(&message_queue, MESSAGE_LENGTH * sizeof(uint8_t), MESSAGE_QUEUE_LENGTH);
    queue_init(&free_queue, MESSAGE_LENGTH * sizeof(uint8_t), MESSAGE_QUEUE_LENGTH);

    message_t message_bank[MESSAGE_QUEUE_LENGTH];
    for (size_t i = 0; i < MESSAGE_QUEUE_LENGTH; i++)
    {
        queue_add_blocking(&free_queue, &message_bank[i]);
    }

    // init uart
    gpio_pull_up(UART_RX_PIN);
    gpio_pull_up(UART_TX_PIN);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_init(uart0, 921600);

    // init PIO
    gpio_pull_up(CS_IN_START);
    gpio_pull_up(CS_IN_START + 1);

    uint offset = pio_add_program(pio0, &trigger_program);
    trigger_program_init(pio0, 0, offset, TRIGGER_PIN);
    pio_sm_set_enabled(pio0, 0, true);

    offset = pio_add_program(pio0, &selecter_program);
    selecter_program_init(pio0, 1, offset, CS_IN_START, CS_OUT_START);
    pio_sm_set_enabled(pio0, 1, true);

    uint32_t val = (clock_get_hz(clk_sys) / (FREQ)) - 1024 - 5;
    pio0->txf[0] = val;

    absolute_time_t time = get_absolute_time();

    // empty uart

    while (to_ms_since_boot(get_absolute_time()) < 5000)
    {
        for (size_t i = 0; i < 10; i++)
        {
            if (uart_is_readable(uart0))
            {
                uart_getc(uart0);
            }
        }
        gpio_put(25, 1);
        sleep_ms(30);
        gpio_put(25, 0);
        sleep_ms(30);
    }
    // start pps interrupt
    gpio_set_irq_enabled_with_callback(PPS_PIN, GPIO_IRQ_EDGE_FALL, true, &interrupt);
    // run core 1
    multicore_launch_core1(core1_entry);
    add_alarm_in_ms(200, alarm_callback, NULL, false);

    uint64_t stamp;
    for (uint64_t message_id = 0;; message_id += DG_PER_MESSAGE)
    {

        message_t message;
        if (queue_is_empty(&free_queue))
            queue_remove_blocking(&message_queue, &message);
        else
            queue_remove_blocking(&free_queue, &message);

        while (!uart_is_readable(uart0))
            tight_loop_contents();
        stamp = last_pps_time;
        uart_read_blocking(uart0, message.data, (DG_LEN * DG_PER_MESSAGE));

        memcpy(&message.data[MESSAGE_LENGTH - 16], &stamp, sizeof(stamp));
        memcpy(&message.data[MESSAGE_LENGTH - 8], &message_id, sizeof(message_id));
        queue_add_blocking(&message_queue, &message);
    }
}
