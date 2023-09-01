#pragma once
#include "hardware/pwm.h"

void init_pwm()
{
    uint slice_num = pwm_gpio_to_slice_num(2);

    gpio_set_function(2, GPIO_FUNC_PWM);
    gpio_set_function(3, GPIO_FUNC_PWM);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_output_polarity(&config, true, true);
    pwm_config_set_phase_correct(&config, true);
    pwm_init(slice_num, &config, false);

    pwm_set_wrap(slice_num, 50000);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 500);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 500);
    pwm_set_enabled(slice_num, true);
}