
#pragma once
#include "uart.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "queues.hpp"

#define pio pio0

static inline int add_receiver(uint pin, uint baud)
{

    static uint offset;
    static bool added = false;
    if (!added)
    {
        offset = pio_add_program(pio, &uart_rx_program);
        added = true;
    }

    int sm = pio_claim_unused_sm(pio, true);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);
    pio_gpio_init(pio, pin);
    // gpio_pull_up(pin);

    pio_sm_config c = uart_rx_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin); // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin); // for JMP
    sm_config_set_in_shift(&c, true, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    float div = (float)clock_get_hz(clk_sys) / (8 * baud);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    return sm;
}

static uint8_t read(int sm)
{
    uint32_t data = pio_sm_get_blocking(pio, sm);
    return (uint8_t)(data);
}

static inline reader_t get_reader(uint pin, uint baud, uint8_t id)
{

    reader_t reader = {
        .id = id,
        .sm = add_receiver(pin, baud),
        .dma_chans = {dma_claim_unused_channel(true), dma_claim_unused_channel(true)},
        // .dma_config = config,
        .current = 0};
    for (int i = 0; i < 2; i++)
    {
        dma_channel_config config = dma_channel_get_default_config((i + 1) % 2);
        channel_config_set_read_increment(&config, false);
        channel_config_set_write_increment(&config, true);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_dreq(&config, pio_get_dreq(pio, reader.sm, false));
        reader.dma_config[i] = config;

        dma_channel_configure(
            reader.dma_chans[i],
            &reader.dma_config[i],
            reader.data[i],
            &pio->rxf[reader.sm],
            chunk_size,
            false);
    }
    dma_channel_start(reader.dma_chans[0]);
    return reader;
}

static inline bool reader_ready(reader_t *reader)
{
    return !dma_channel_is_busy(reader->dma_chans[reader->current]);
}

static inline void reader_switch(reader_t *reader)
{
    int i = reader->current;
    dma_channel_configure(
        reader->dma_chans[i],
        &reader->dma_config[i],
        reader->data[i],
        &pio->rxf[reader->sm],
        chunk_size,
        false);
    reader->current = (reader->current + 1) % 2;
}