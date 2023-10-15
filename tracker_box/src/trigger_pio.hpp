#pragma once
#include "trigger.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

static PIO trigger_pio;
static int trigger_sm;

static inline void trigger_pio_init()
{
    // const auto pin_tov = 19;
    const auto pin_trigger = 10;
    const auto pin_stamp = 11;

    trigger_pio = pio0;

    trigger_sm = pio_claim_unused_sm(trigger_pio, true);
    uint offset = pio_add_program(trigger_pio, &trigger_program);
    pio_sm_config c = trigger_program_get_default_config(offset);

    pio_sm_set_pins(trigger_pio, trigger_sm, 31);
    pio_sm_set_consecutive_pindirs(trigger_pio, trigger_sm, pin_trigger, 2, true);
    pio_gpio_init(trigger_pio, pin_trigger);
    pio_gpio_init(trigger_pio, pin_stamp);

    // set_pins
    sm_config_set_set_pins(&c, pin_trigger, 2);

    // clock speed
    float div = clock_get_hz(clk_sys) / 1e6;
    sm_config_set_clkdiv(&c, div);

    //  init
    pio_sm_init(trigger_pio, trigger_sm, offset, &c);

    pio_sm_put(trigger_pio, trigger_sm, (uint32_t)(60 * 100 - 1)); // readings without stamp
    pio_sm_put(trigger_pio, trigger_sm, (uint32_t)10000 - 25);     // delay between readings us
}

int64_t start(alarm_id_t id, void *user_data)
{
    pio_sm_set_enabled(trigger_pio, trigger_sm, true);
    return 0;
}

static inline void trigger_start()
{
    add_alarm_in_ms(10000, start, NULL, false);
}

static inline uint32_t trigger_pio_get()
{
    if (pio_sm_get_rx_fifo_level(trigger_pio, trigger_sm) > 0)
        return pio_sm_get(trigger_pio, trigger_sm);
    return 0;
}