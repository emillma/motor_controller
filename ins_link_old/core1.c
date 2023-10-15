#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/util/queue.h"

#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "header.h"

void core1_entry()
{

    spi_init(spi1, 5000 * 1000);
    spi_set_slave(spi1, true);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
    for (int i = 12; i <= 15; i++)
    {
        gpio_set_function(i, GPIO_FUNC_SPI);
    }
    // // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(15, 14, 13, 12, GPIO_FUNC_SPI));

    while (!gpio_get(13))
        tight_loop_contents();
    while (true)
    {
        message_t message;
        message_t input;

        queue_remove_blocking(&message_queue, &message);
        while (!gpio_get(13)) // wait for deselect before allow selection
            tight_loop_contents();
        while (gpio_get(13))
            tight_loop_contents();
        gpio_put(25, 1);
        gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
        spi_write_blocking(spi1, message.data, sizeof(message.data));
        gpio_set_function(MISO_PIN, GPIO_FUNC_NULL);

        // TODO fix this
        // if (input.data[0] == 0xb && input.data[1] == 0 && input.data[2] == 0 && input.data[3] == 7)
        //     watchdog_reboot(0, 0, 0);
        gpio_put(25, 0);
        queue_add_blocking(&free_queue, &message);
    }
}
