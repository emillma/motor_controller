#pragma once
#include "encoder.pio.h"

#define pio pio0
#define pin 2

static int sm;
static int val;
static inline void encoder_program_init()
{
    sm = pio_claim_unused_sm(pio, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 3, false);
    for (uint i = 0; i < 3; ++i)
        pio_gpio_init(pio, pin + i);

    uint offset = pio_add_program(pio, &encoder_program);
    pio_sm_config c = encoder_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin);
    sm_config_set_in_shift(&c, false, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    pio_sm_get_blocking(pio, sm);
}

static inline uint32_t encoder_read_blocking()
{
    uint32_t data = pio_sm_get_blocking(pio, sm);
    if (data & 1 << 4)
        val = 0;
    else if ((data ^ data >> 5) & 1)
        val += 1;
    else
        val -= 1;
    return val;
}
