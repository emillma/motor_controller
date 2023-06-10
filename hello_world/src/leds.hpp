#include "pico/stdlib.h"

#pragma once

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

void blink_fast()
{
    uint32_t time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) < time + 1000)
    {
        blink_for(50);
        sleep_ms(100);
    }
}
