#pragma once
#include "invert.pio.h"

#define pio pio0
#define pin_in 2
#define pin_out 4

static inline void invert_program_init()
{
    int sm = pio_claim_unused_sm(pio, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin_out, 1, true);
    pio_gpio_init(pio, pin_out);

    uint offset = pio_add_program(pio, &invert_program);
    pio_sm_config c = invert_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_in);
    sm_config_set_out_pins(&c, pin_out, 1);
    // sm_config_set_set_pins(&c, pin_out, 1);

    // sm_config_set_in_shift(&c, false, false, 32);
    // sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    // pio_sm_get_blocking(pio, sm);
}