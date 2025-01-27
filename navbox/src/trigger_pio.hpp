#pragma once
#include "trigger.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#define trigger_pio pio0

static int trigger_sm;

static inline void init_trigger_pio()
{
    const auto pin_tov = 19;
    const auto pin_trigger = 20;
    const auto pin_stamp = 21;

    const auto pin_led = 25;

    trigger_sm = pio_claim_unused_sm(trigger_pio, true);
    uint offset = pio_add_program(trigger_pio, &trigger_program);
    pio_sm_config c = trigger_program_get_default_config(offset);

    pio_gpio_init(trigger_pio, pin_trigger);
    pio_gpio_init(trigger_pio, pin_stamp);
    pio_sm_set_consecutive_pindirs(trigger_pio, trigger_sm, pin_trigger, 2, true);
    // gpio_pull_up(pin_tov);

    // in_pins
    sm_config_set_in_pins(&c, pin_tov);

    // out_pins
    sm_config_set_out_pins(&c, pin_trigger, 2);

    // set_pins
    sm_config_set_set_pins(&c, pin_trigger, 2);

    // clock speed
    float div = clock_get_hz(clk_sys) / 1e7;
    sm_config_set_clkdiv(&c, div);

    sm_config_set_in_shift(&c, false, false, 32);

    //  init
    pio_sm_set_pins(trigger_pio, pin_trigger, 1);
    pio_sm_init(trigger_pio, trigger_sm, offset, &c);
    pio_sm_put(trigger_pio, trigger_sm, (uint32_t)(4096 >> 7));
}

static int64_t trigger_start(alarm_id_t id, void *user_data)
{
    pio_sm_set_enabled(trigger_pio, trigger_sm, true);
    return 0;
}

static inline uint32_t trigger_pio_get()
{
    if (pio_sm_get_rx_fifo_level(trigger_pio, trigger_sm) > 0)
        return pio_sm_get(trigger_pio, trigger_sm);
    return 0;
}
