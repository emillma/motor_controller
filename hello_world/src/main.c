#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
int main()
{
    // Initialise I/O
    stdio_init_all();

    // initialise GPIO (Green LED connected to pin 25)
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // Main Loop
    while (1)
    {
        gpio_put(25, 1); // Set pin 25 to high
        printf("LED ON!\n");
        // get current time as float
        float time = to_us_since_boot(get_absolute_time()) / 1000000.0f;
        printf("Time: %f", time);

        sleep_ms(1000); // 0.5s delay

        gpio_put(25, 0); // Set pin 25 to low
        printf("LED OFF!\n");
        sleep_ms(1000); // 0.5s delay
    }
}
