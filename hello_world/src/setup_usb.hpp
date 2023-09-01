#pragma once
#include <iostream>
#include "pico/stdlib.h"
#include "pico/stdio.h"

void init_usb()
{
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false);
}
void handle_usb_in()
{
    char c;
    bool any = false;
    for (int out = getchar_timeout_us(0); out != PICO_ERROR_TIMEOUT; out = getchar_timeout_us(10))
    {
        any = true;
        c = (char)out;
        fwrite(&c, 1, 1, stdout);
    }
    if (any)
        fflush(stdout);
}
