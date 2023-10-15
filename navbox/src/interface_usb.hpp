#pragma once
#include <iostream>
#include "pico/stdlib.h"
#include "pico/stdio.h"

void usb_init()
{
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false);
    // add_alarm_in_ms(100, usb_flush_alarm_callback, NULL, true);
}
void usb_send_id(uint8_t id)
{
    const uint8_t header[] = {0xfe, id};
    fwrite(header, 1, 2, stdout);
}

void usb_send_stuffed(const uint8_t *data, size_t len)
{
    const uint8_t marker = 0xfe;
    const uint8_t escape = 0xff;
    for (size_t idx = 0; idx < len; idx++)
    {
        fwrite(&data[idx], 1, 1, stdout);
        if (data[idx] == marker)
        {
            fwrite(&escape, 1, 1, stdout);
        }
    }
}
void usb_flush()
{
    fflush(stdout);
}

void usb_send_byte(uint8_t byte)
{
    fwrite(&byte, 1, 1, stdout);
}