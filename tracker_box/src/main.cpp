#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"

#include "leds.hpp"
#include "mpu6050.hpp"
#include "uart_pio.hpp"
#include "button.hpp"

int main()
{
    FRESULT fr;
    FATFS fs;
    DIR dir;
    FIL fil;

    char dirname[32];
    char filename[64];

    stdio_init_all();
    init_led();
    i2c_init();

    sleep_ms(1000);

    sd_init_driver();
    fr = f_mount(&fs, "0:", 1);
    bool success = false;
    for (int i = 0; i < 100; i++)
    {
        sprintf(dirname, "0:/dir_%02d", i);
        if (f_mkdir(dirname) == FR_OK)
        {
            printf("Created directory %s", dirname);
            success = true;
            break;
        }
    }
    if (!success)
    {
        printf("Failed to create directory");
        led_error(200);
    }
    int count = 0;
    uint8_t data[4096];
    for (int i = 0; i < 4096; i++)
    {
        data[i] = i % 256;
    }
    while (!get_bootsel_button())
    {
        sprintf(filename, "%s/file_%06d.bin", dirname, count++);
        f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
        f_write(&fil, data, 4096, NULL);
        f_close(&fil);
        sleep_ms(1000);
    }
    // f_op
    f_unmount("0:");
    led_error(1000);
}