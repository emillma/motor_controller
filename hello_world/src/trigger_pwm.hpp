#pragma once
#include "hardware/pwm.h"

void pwm_init()
{
    uint pin = 18;
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint chan_num = pwm_gpio_to_channel(pin);
    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 133.0f);
    pwm_config_set_output_polarity(&config, true, true);
    pwm_init(slice_num, &config, false);
    pwm_set_wrap(slice_num, 10000);
    pwm_set_chan_level(slice_num, chan_num, 100);
    pwm_set_enabled(slice_num, true);
}