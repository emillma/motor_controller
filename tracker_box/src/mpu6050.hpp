#pragma once
// #include <algorithm>
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// bool reserved_addr(uint8_t addr)
// {
//     return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
// }

#define i2c_imu i2c1
#define i2c_imu_sda 26
#define i2c_imu_scl 27
#define i2c_imu_addr 0x68

int imu_write_reg(uint8_t reg, uint8_t byte)
{
    uint8_t data[] = {reg, byte};
    return i2c_write_blocking(i2c_imu, i2c_imu_addr, data, 2, false);
}

void i2c_init()
{

    i2c_init(i2c_imu, 400 * 1000);
    gpio_pull_up(i2c_imu_scl);
    gpio_pull_up(i2c_imu_sda);
    // reset_i2c();
    gpio_set_function(i2c_imu_scl, GPIO_FUNC_I2C);
    gpio_set_function(i2c_imu_sda, GPIO_FUNC_I2C);
    bi_decl(bi_2pins_with_func(i2c_imu_sda, i2c_imu_scl, GPIO_FUNC_I2C));

    imu_write_reg(0x6b, 0b10000000); // Reset device
    sleep_ms(100);
    imu_write_reg(0x6b, 0b00000111); // Reset sensors
    sleep_ms(100);
    imu_write_reg(0x6b, 0b00000001); // Internal Clock set to Gyro output
    imu_write_reg(0x6a, 0b00000100); // Reset FIFO

    imu_write_reg(0x19, 7); // rate devider 8kHz/(1+7) = 1kHz
    imu_write_reg(0x23, 0b11111100);
    imu_write_reg(0x37, 0b00100100);
    imu_write_reg(0x38, 0b00000001);
    imu_write_reg(0x1a, 0b00001000);
}

typedef struct
{
    uint8_t data[1024];
    uint8_t id;
    int32_t size;
} f9p_message_t;

uint8_t imu_read_reg(uint8_t reg)
{
    uint8_t out;
    i2c_write_blocking(i2c_imu, i2c_imu_addr, &reg, 1, true);
    i2c_read_blocking(i2c_imu, i2c_imu_addr, &out, 1, false);
    return out;
}

uint16_t imu_read_fifo(uint8_t *buf)
{
    uint8_t reg = 0x72;
    uint8_t len[2];
    i2c_write_timeout_us(i2c_imu, i2c_imu_addr, &reg, 1, true, 100);
    i2c_read_timeout_us(i2c_imu, i2c_imu_addr, len, 2, false, 100);
    imu_read_reg(58);
    // i2c_write_timeout_us(i2c_imu, i2c_imu_addr, buf,
    return len[0] << 8 | len[1];
}
