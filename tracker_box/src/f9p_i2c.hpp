#pragma once
#include <algorithm>
#include "pico/binary_info.h"

#include "hardware/i2c.h"

// bool reserved_addr(uint8_t addr)
// {
//     return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
// }

#define i2c_f9p i2c1
#define i2c_f9p_sda 26
#define i2c_f9p_scl 27
#define f9p_max_size 2048
typedef struct
{
    uint8_t data[f9p_max_size];
    uint8_t id;
    int32_t size;
} f9p_message_t;

static void reset_i2c()
{
    gpio_put(i2c_f9p_scl, 0);
    gpio_set_function(i2c_f9p_scl, GPIO_FUNC_SIO);
    gpio_set_dir(i2c_f9p_sda, GPIO_OUT);

    gpio_set_function(i2c_f9p_sda, GPIO_FUNC_SIO);
    gpio_set_dir(i2c_f9p_sda, GPIO_IN);

    for (int j = 0; j < 9; j++)
    {
        gpio_put(i2c_f9p_sda, 1);
        sleep_us(10);
        gpio_put(i2c_f9p_sda, 0);
        sleep_us(10);
    }
    sleep_us(2);
    gpio_put(i2c_f9p_sda, 1);
    sleep_us(2);
    gpio_set_dir(i2c_f9p_sda, GPIO_OUT);
    gpio_put(i2c_f9p_sda, 1);

    sleep_us(100);
}

void i2c_init()
{

    i2c_init(i2c_f9p, 400 * 1000);
    gpio_set_pulls(i2c_f9p_sda, false, false);
    reset_i2c();
    gpio_set_function(i2c_f9p_scl, GPIO_FUNC_I2C);
    gpio_set_function(i2c_f9p_sda, GPIO_FUNC_I2C);
    bi_decl(bi_2pins_with_func(i2c_f9p_sda, i2c_f9p_scl, GPIO_FUNC_I2C));
}

bool f9p_forward(uint8_t addr, f9p_message_t *msg)
{
    uint8_t available_rxdata[2];
    const uint8_t reg = 0xfd;
    int available;

    msg->id = addr;

    i2c_write_timeout_us(i2c_f9p, addr, &reg, 1, true, 1000);
    i2c_read_timeout_us(i2c_f9p, addr, available_rxdata, 2, true, 1000);

    available = (available_rxdata[0] << 8 | available_rxdata[1]);
    if (available == 0xffff)
        available = 0;
    else if (available > f9p_max_size)
        available = f9p_max_size;

    int read = i2c_read_timeout_per_char_us(i2c_f9p, addr, msg->data, available, false, 10);
    msg->size = read >= 0 ? (int32_t)read : 0;
    return available > 0;
}
