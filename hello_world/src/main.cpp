#include <regex>
#include <string>

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "usb_setup.hpp"
#include "leds.hpp"
#include "pio_setup.hpp"
#include "pwm_setup.hpp"
#include "stim_setup.hpp"
#include "i2c_setup.hpp"
#include "hardware/watchdog.h"

void handle_usb_input()
{

    static std::string data;
    bool any = false;
    const std::regex re("\xff([\x01-\xfd])([^]*?)(?=\xff[^\xfe])");
    const std::regex stuff("\xff\xfe");
    std::smatch match;
    std::string msg;

    for (int c = getchar_timeout_us(100); c != PICO_ERROR_TIMEOUT; c = getchar_timeout_us(100))
    {
        any = true;
        data += c;
    }
    if (!any)
        return;
    while (std::regex_search(data, match, re))
    {
        data = match.suffix().str();
        uint8_t type = match[1].str().c_str()[0];
        msg = std::regex_replace(match[2].str(), stuff, "\xff");
        if (type == 1) // watchdog
        {
            watchdog_update();
        }

        else if (type == 2) // Echo
        {
            usb_send_id(2);
            usb_send_stuffed((uint8_t *)msg.c_str(), msg.size());
            usb_flush();
        }
        else if (type == 3) // Stim
        {
            printf("to stim: %s", msg.c_str());
            stim_write((uint8_t *)msg.c_str(), msg.size());
        }
    }
}

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    blink_fast();

    usb_init();
    stim_init();
    // pwm_init();
    pio_inverter_init();
    i2c_init();

    watchdog_enable(5000, 1);

    while (true)
    {
        watchdog_update();
        for (int c = getchar_timeout_us(10); c != PICO_ERROR_TIMEOUT; c = getchar_timeout_us(10))
        {
            stim_write((uint8_t *)&c, 1);
        }
        stim_forward();
        usb_flush();
        // f9p_a_forward();
        // f9p_b_forward();
    }
}
