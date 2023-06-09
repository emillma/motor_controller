#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/stdio_usb.h"
#include "pico/stdio.h"

#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/irq.h"
#include "leds.hpp"

struct MyStructure
{
    bool waiting;
    int last_int;
    char last_char;
};

bool repeating_timer_callback(struct repeating_timer *t)
{
    uint32_t time = to_ms_since_boot(get_absolute_time());
    // printf("Timer %lu\n", time);
    return true;
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

int main()
{
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    struct repeating_timer timer;
    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);

    struct MyStructure my_structure;
    my_structure.waiting = true;
    stdio_set_chars_available_callback(handle_usb_input, (void *)&my_structure);
    getchar_timeout_us(100);

    // irq_add_shared_handler(USBCTRL_IRQ, usbctrl_irq_handler, 0);
    // irq_set_enabled(USBCTRL_IRQ, true);

    while (true)
    {

        // printf("Hello, world!\n");
        sleep_ms(2000);
    }
}
