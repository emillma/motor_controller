#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/stdio_usb.h"
#include "pico/stdio.h"

#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/irq.h"

#include "external.h"

bool repeating_timer_callback(struct repeating_timer *t)
{
    // current time
    uint64_t time = get_absolute_time();
    printf("Timer %llu\n", time);

    return true;
}

int64_t turn_off(alarm_id_t id, void *user_data)
{
    gpio_put(25, 0);
    return 0;
}

void blink_for(uint32_t ms)
{
    gpio_put(25, 1);
    add_alarm_in_ms(ms, turn_off, NULL, false);
}

void process_data(void *param)
{
    blink_for(50);
    printf("Data available!\n");
}

void blink_fast()
{
    uint64_t time = get_absolute_time();
    while (get_absolute_time() - time < (int)(0.5 * 1000000))
    {
        blink_for(50);
        sleep_ms(100);
    }
}

void usbctrl_irq_handler()
{
    printf("USB IRQ\n");
}

int main()
{
    stdio_init_all();
    stdio_set_chars_available_callback(process_data, NULL);
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    printf("Hello, world!\n");
    struct repeating_timer timer;

    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);
    getchar_timeout_us(100);

    // irq_add_shared_handler(USBCTRL_IRQ, usbctrl_irq_handler, 0);
    // irq_set_enabled(USBCTRL_IRQ, true);

    char hello;
    while (true)
    {
        sleep_ms(10);

        scanf("%c", &hello);
        // if (hello == 'R')
        // {
        //     printf("Resetting...\n");
        //     reset_usb_boot(0, 0);
        // }
    }
}
