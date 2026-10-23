#include <hardware/PulseWidthModulation.hpp>
#include "platform/pwm.h"

PulseWidthModulation::PulseWidthModulation(uint8_t gpio_pin, float frequency, float duty_cycle, bool inverted)
    : gpio_pin(gpio_pin)
{
    if (mcu_pwm_init(gpio_pin, frequency, inverted))
        mcu_pwm_set_duty(gpio_pin, duty_cycle);
}

void PulseWidthModulation::SetFrequency(float frequency)
{
    mcu_pwm_set_frequency(gpio_pin, frequency);
}

void PulseWidthModulation::SetDutyCycle(float duty_cycle)
{
    mcu_pwm_set_duty(gpio_pin, duty_cycle);
}

void PulseWidthModulation::SetEnabled(bool enable)
{
    mcu_pwm_set_enabled(gpio_pin, enable);
}