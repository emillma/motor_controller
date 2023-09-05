#pragma once
#include <iostream>
#include "pico/stdlib.h"
#include "pico/stdio.h"

void usb_init()
{
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false);
}
void usb_handle_input()
{

    bool any = false;
    for (int input = getchar_timeout_us(0); input != PICO_ERROR_TIMEOUT; input = getchar_timeout_us(0))
    {
        stim_send_char((char)input);
    }
}
