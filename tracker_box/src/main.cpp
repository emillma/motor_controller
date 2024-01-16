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
    // set max amps

    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);
    gpio_put(20, 1);

    sleep_ms(1000);

    sd_init_driver();
    fr = f_mount(&fs, "0:", 1);
    for (int i = 0; i < 100; i++)
    {
        sprintf(dirname, "0:/dir_%02d", i);
        if (f_mkdir(dirname) == FR_OK)
        {
            printf("Created directory %s", dirname);
            break;
        }
    }

    int count = 0;
    reader_t reader = get_reader(26, 921600, 26);
    sprintf(filename, "%s/data_%06d.bin", dirname, count++);
    f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);

    while (!get_bootsel_button())
    {
        if (reader_ready(reader))
            if (true)
            {
                led_set(count++ % 2);
                f_write(&fil, reader_switch(reader), chunk_size, NULL);
            }
        sleep_us(100);
    }
    f_close(&fil);
    f_unmount("0:");
    led_error(1000);
}