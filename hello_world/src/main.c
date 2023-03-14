#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/stdio_usb.h"
#include "pico/stdio.h"

#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/irq.h"

#include "external.h"

struct MyStructure
{
    bool waiting;
    int last_int;
    char last_char;
};

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

int64_t do_stuff(alarm_id_t id, void *data_ptr)

{
    struct MyStructure *data = (struct MyStructure *)data_ptr;
    for (int c = getchar_timeout_us(0); c != PICO_ERROR_TIMEOUT; c = getchar_timeout_us(0))
    {
        printf("%c", (char)c);
    }
    printf("\n");
    data->waiting = true;
    return 0;
}

void handle_usb_input(void *data_ptr)
{
    struct MyStructure *data = (struct MyStructure *)data_ptr;
    if (data->waiting)
    {
        data->waiting = false;
        add_alarm_in_ms(1, do_stuff, data, true);
    }
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

int main()
{
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    struct repeating_timer timer;
    // add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);

    struct MyStructure my_structure;
    my_structure.waiting = true;
    stdio_set_chars_available_callback(handle_usb_input, (void *)&my_structure);
    // getchar_timeout_us(100);

    // irq_add_shared_handler(USBCTRL_IRQ, usbctrl_irq_handler, 0);
    // irq_set_enabled(USBCTRL_IRQ, true);

    while (true)
    {
        sleep_ms(2000);
    }
}
