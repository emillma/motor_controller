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

queue_t ab_queue;
queue_t ba_queue;

typedef struct
{
    uint8_t data[1024];
    int32_t len;
} f9p_message_t;

f9p_message_t f9p_slots[8];

__attribute__((aligned(32))) 
static uint8_t stim_buffer[2][STIM_BUFFER_SIZE];

void core1_entry()
{


}

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    queue_init(&ab_queue, sizeof(f9p_message_t), 8);
    queue_init(&ba_queue, sizeof(f9p_message_t), 8);

    // for (int i =0; i<8;i++)
    //     queue_add_blocking(&ab_queue, &f9p_slots[i]);
        
    usb_init();
    // stim_init(stim_buffer);
    pwm_init();
    pio_inverter_init();
    i2c_init();

    multicore_launch_core1(core1_entry);
    watchdog_enable(5000, 1);

    while (true)
    {
        watchdog_update();
        // stim_forward(stim_buffer);

        // sleep_ms(500);
    }
}
