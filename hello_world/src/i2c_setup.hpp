#pragma once
#include "pico/binary_info.h"

#include "hardware/i2c.h"

// bool reserved_addr(uint8_t addr)
// {
//     return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
// }

#define i2c_fp9 i2c1
#define i2c_f9p_sda 26
#define i2c_f9p_scl 27

void i2c_init()
{

    i2c_init(i2c_fp9, 400 * 1000);
    gpio_set_function(i2c_f9p_sda, GPIO_FUNC_I2C);
    gpio_set_function(i2c_f9p_scl, GPIO_FUNC_I2C);
    gpio_pull_up(i2c_f9p_sda);
    gpio_pull_up(i2c_f9p_scl);
    // PICO_DEFAULT_I2C_SDA_PIN;
    bi_decl(bi_2pins_with_func(i2c_f9p_sda, i2c_f9p_scl, GPIO_FUNC_I2C));
}

void i2c_forward(uint8_t addr, uint8_t id)
{
    uint8_t rxdata[4096];
    const uint8_t reg = 0xfd;
    int32_t available;
    i2c_write_blocking(i2c_fp9, addr, &reg, 1, true);
    i2c_read_blocking(i2c_fp9, addr, rxdata, 2, false);
    available = rxdata[0] << 8 | rxdata[1];
    if (available > 0)
        usb_send_id(id);
    while (available > 0)
    {
        if (available > 4096)
        {
            i2c_read_blocking(i2c_fp9, addr, rxdata, 4096, false);
            usb_send_stuffed(rxdata, 4096);
            available -= 4096;
        }
        else
        {
            i2c_read_blocking(i2c_fp9, addr, rxdata, available, false);
            usb_send_stuffed(rxdata, available);
            available = 0;
        }
    }
}

void f9p_a_forward()
{
    i2c_forward(30, 30);
}

void f9p_b_forward()
{
    i2c_forward(31, 31);
}