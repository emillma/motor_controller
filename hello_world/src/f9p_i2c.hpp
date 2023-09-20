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
    gpio_pull_up(i2c_f9p_scl);
    gpio_pull_up(i2c_f9p_sda);
    reset_i2c();
    gpio_set_function(i2c_f9p_scl, GPIO_FUNC_I2C);
    gpio_set_function(i2c_f9p_sda, GPIO_FUNC_I2C);
    bi_decl(bi_2pins_with_func(i2c_f9p_sda, i2c_f9p_scl, GPIO_FUNC_I2C));
}

typedef struct
{
    uint8_t data[1024];
    uint8_t id;
    int32_t size;
} f9p_message_t;

bool i2c_forward(uint8_t addr, f9p_message_t *msg)
{
    uint8_t available_rxdata[2];
    const uint8_t reg = 0xfd;
    int available;

    i2c_write_timeout_us(i2c_f9p, addr, &reg, 1, true, 1000);
    i2c_read_timeout_us(i2c_f9p, addr, msg->data, 2, true, 1000);

    available = (msg->data[0] << 8 | msg->data[1]);
    if (available == 0xffff)
        available = 0;
    else if (available > 1022)
        available = 1022;

    i2c_read_timeout_us(i2c_f9p, addr, &msg->data[2], available, false, 10000);

    // i2c_write_timeout_us(i2c_fp9, addr, &reg, 1, true, 1000);
    // i2c_read_timeout_us(i2c_fp9, addr, msg->data, available, false, 10000);
    msg->size = available;
    return available > 0 ? true : false;
}

bool f9p_a_forward(f9p_message_t *msg)
{
    msg->id = 30;
    return i2c_forward(30, msg);
}

bool f9p_b_forward(f9p_message_t *msg)
{
    msg->id = 31;
    return i2c_forward(31, msg);
}
