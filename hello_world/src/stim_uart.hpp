#pragma once
#include "hardware/uart.h"
#include <hardware/dma.h>

// stim struct

typedef struct
{
    uint8_t data[38 * 256];
} stim_message_t;

static const size_t stim_message_size = sizeof(stim_message_t);
extern stim_message_t stim_buffers[2];

static void prep_dma(uint dma_channel, uint chain_to, stim_message_t *stim_buffer)
{
    const auto dreq = DREQ_UART1_RX;
    const auto uart_hw = uart1_hw;
    static dma_channel_config config = {0};
    if (config.ctrl == 0)
    {
        config = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_write_increment(&config, true);
        channel_config_set_read_increment(&config, false);
        channel_config_set_dreq(&config, dreq);
        dma_channel_abort(dma_channel);
    }
    channel_config_set_chain_to(&config, chain_to);
    dma_channel_configure(
        dma_channel,
        &config,
        stim_buffer->data,
        &uart_hw->dr,
        stim_message_size,
        false);
}

void stim_init()
{
    const auto uart = uart1;
    const int baud_rate = 1843200;
    const int pin0 = 8;
    const int pin1 = 9;

    uart_init(uart, baud_rate);
    gpio_set_function(pin0, GPIO_FUNC_UART);
    gpio_set_function(pin1, GPIO_FUNC_UART);

    prep_dma(0, 1, &stim_buffers[0]);
    prep_dma(1, 0, &stim_buffers[1]);
    dma_channel_start(0);
}

void stim_forward()
{
    static int current = 0;
    if (!dma_channel_is_busy(current))
    {
        usb_send_id(90);
        usb_send_stuffed(stim_buffers[current].data, stim_message_size);
        prep_dma(current, (current + 1) % 2, &stim_buffers[current]);
        current = (current + 1) % 2;
        // usb_flush();
    }
}
