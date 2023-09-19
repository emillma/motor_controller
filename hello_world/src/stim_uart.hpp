#pragma once
#include "hardware/uart.h"
#include <hardware/dma.h>

#define STIM_BUFFER_SIZE 4096
static void prep_dma(uint dma_channel, uint chain_to, uint8_t *stim_buffer)
{
    const auto dreq = DREQ_UART1_RX;
    const auto uart_hw = uart1_hw;

    dma_channel_config config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_write_increment(&config, true);
    channel_config_set_read_increment(&config, false);
    channel_config_set_dreq(&config, dreq);
    channel_config_set_chain_to(&config, chain_to);
    dma_channel_configure(
        dma_channel,
        &config,
        stim_buffer,
        &uart_hw->dr,
        STIM_BUFFER_SIZE,
        false);
}

void stim_init(uint8_t stim_buffer[2][STIM_BUFFER_SIZE])
{
    const auto uart = uart1;
    const int baud_rate = 1843200;
    const int pin0 = 8;
    const int pin1 = 9;

    uart_init(uart, baud_rate);
    gpio_set_function(pin0, GPIO_FUNC_UART);
    gpio_set_function(pin1, GPIO_FUNC_UART);

    prep_dma(0, 1, stim_buffer[0]);
    prep_dma(1, 0, stim_buffer[1]);
    dma_channel_start(0);
}

void stim_forward(uint8_t stim_buffer[2][STIM_BUFFER_SIZE])
{
    static int current = 0;
    if (dma_channel_hw_addr(current)->transfer_count == 0)
    {
        usb_send_id(90);
        usb_send_stuffed(stim_buffer[current], STIM_BUFFER_SIZE);
        prep_dma(current, (current + 1) % 2, stim_buffer[current]);
        current = (current + 1) % 2;
    }
    // uint32_t count = dma_channel_hw_addr(0)->transfer_count;
}

// void stim_write(uint8_t *data, size_t len)
// {
//     uart_write_blocking(stim_uart_id, data, len);
// }
