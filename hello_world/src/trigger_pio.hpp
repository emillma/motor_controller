#pragma once
#include "trigger.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

PIO trigger_pio;
int trigger_sm;

static inline void trigger_pio_init()
{
    const auto pin_jmp = 18;
    const auto pin_out = 19;
    const auto pin_in = 20;

    gpio_init(pin_in);
    gpio_set_dir(pin_in, GPIO_IN);

    trigger_pio = pio0;

    trigger_sm = pio_claim_unused_sm(trigger_pio, true);
    pio_sm_set_consecutive_pindirs(trigger_pio, trigger_sm, pin_out, 1, true);

    uint offset = pio_add_program(trigger_pio, &trigger_program);
    pio_sm_config c = trigger_program_get_default_config(offset);

    sm_config_set_in_pins(&c, pin_in);
    sm_config_set_jmp_pin(&c, pin_jmp);
    gpio_pull_up(pin_jmp);

    pio_gpio_init(trigger_pio, pin_out);
    sm_config_set_out_pins(&c, pin_out, 1);
    sm_config_set_set_pins(&c, pin_out, 1);
    sm_config_set_sideset_pins(&c, pin_out);
    // clock_get_hz(clk_sys);
    float div = clock_get_hz(clk_sys) / 1000000.f;
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(trigger_pio, trigger_sm, offset, &c);
    pio_sm_set_pins(trigger_pio, trigger_sm, 1);
}
static inline void trigger_start()
{
    pio_sm_set_enabled(trigger_pio, trigger_sm, true);
}

static inline uint32_t trigger_pio_get()
{
    if (pio_sm_get_rx_fifo_level(trigger_pio, trigger_sm) > 0)
        return pio_sm_get(trigger_pio, trigger_sm);
    return -1;
}