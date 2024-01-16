
#pragma once
#include "uart.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#define pio_uart pio1
#define chunk_size 4096

typedef struct
{
    uint8_t id;
    int sm;
    int dma_chans[2];
    uint8_t data[2][chunk_size];
    dma_channel_config dma_config[2];
    uint32_t current;
} reader_t;

static inline int add_receiver(uint pin, uint baud)
{

    static uint offset;
    static bool added;
    if (!added)
    {
        offset = pio_add_program(pio_uart, &uart_rx_program);
        added = true;
    }

    int sm = pio_claim_unused_sm(pio_uart, true);
    pio_sm_set_consecutive_pindirs(pio_uart, sm, pin, 1, false);
    pio_gpio_init(pio_uart, pin);
    gpio_pull_up(pin);

    pio_sm_config c = uart_rx_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin); // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin); // for JMP
    sm_config_set_in_shift(&c, true, true, 32);

    float div = (float)clock_get_hz(clk_sys) / (16 * baud);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio_uart, sm, offset, &c);
    pio_sm_set_enabled(pio_uart, sm, true);

    return sm;
}

static inline void configure(reader_t &reader, int i)
{
    dma_channel_configure(
        reader.dma_chans[i],
        &reader.dma_config[i],
        &reader.data[i][4],
        &pio_uart->rxf[reader.sm],
        chunk_size - 4,
        false);
}

static inline reader_t get_reader(uint pin, uint baud, uint8_t id)
{

    reader_t reader = {
        .id = id,
        .sm = add_receiver(pin, baud),
        .dma_chans = {dma_claim_unused_channel(true), dma_claim_unused_channel(true)},
        .current = 0};
    for (int i = 0; i < 2; i++)
    {

        int next = (i + 1) % 2;
        dma_channel_config config = dma_channel_get_default_config(reader.dma_chans[i]);

        channel_config_set_read_increment(&config, false);
        channel_config_set_write_increment(&config, true);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_dreq(&config, pio_get_dreq(pio_uart, reader.sm, false));
        channel_config_set_chain_to(&config, reader.dma_chans[next]);

        reader.dma_config[i] = config;

        reader.data[i][0] = 0xde;
        reader.data[i][1] = 0xad;
        reader.data[i][2] = 0xbe;
        reader.data[i][3] = id;

        configure(reader, i);
    }
    dma_channel_start(reader.dma_chans[0]);
    return reader;
}

static inline bool reader_ready(reader_t &reader)
{
    return !dma_channel_is_busy(reader.dma_chans[reader.current]);
}

static inline uint8_t *reader_switch(reader_t &reader)
{
    int i = reader.current;
    reader.current = (reader.current + 1) % 2;
    configure(reader, i);
    return reader.data[i];
}