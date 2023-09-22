#pragma once
#include "trigger.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

static PIO trigger_pio;
static int trigger_sm;

static inline void trigger_pio_init()
{
    const auto pin_trigger = 19;
    const auto pin_tov = 20;
    const auto pin_pps = 21;

    const auto pin_led = 25;

    trigger_pio = pio0;

    trigger_sm = pio_claim_unused_sm(trigger_pio, true);
    uint offset = pio_add_program(trigger_pio, &trigger_program);
    pio_sm_config c = trigger_program_get_default_config(offset);

    // in_pins
    sm_config_set_in_pins(&c, pin_tov);
    gpio_pull_up(pin_tov);
    // jmp_pin
    sm_config_set_jmp_pin(&c, pin_tov);
    gpio_pull_up(pin_pps);

    // out_pins
    sm_config_set_out_pins(&c, pin_trigger, 1);
    pio_gpio_init(trigger_pio, pin_trigger);
    pio_sm_set_consecutive_pindirs(trigger_pio, trigger_sm, pin_trigger, 1, true);
    pio_sm_set_pins(trigger_pio, pin_trigger, 1);

    // set_pins
    sm_config_set_set_pins(&c, pin_trigger, 1);
    pio_gpio_init(trigger_pio, pin_trigger);
    pio_sm_set_consecutive_pindirs(trigger_pio, trigger_sm, pin_trigger, 1, true);

    // clock speed
    float div = clock_get_hz(clk_sys) / 1e7;
    sm_config_set_clkdiv(&c, div);

    //  init
    pio_sm_init(trigger_pio, trigger_sm, offset, &c);
}
static inline void trigger_start()
{
    pio_sm_set_enabled(trigger_pio, trigger_sm, true);
}

static inline uint32_t trigger_pio_get()
{
    if (pio_sm_get_rx_fifo_level(trigger_pio, trigger_sm) > 0)
        return pio_sm_get(trigger_pio, trigger_sm);
    return 0;
}
