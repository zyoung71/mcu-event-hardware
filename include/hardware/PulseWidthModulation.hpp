#pragma once

#include <platform/pwm.h>

class PulseWidthModulation
{
protected:
    const uint8_t gpio_pin;

public:
    PulseWidthModulation(uint8_t gpio_pin, float frequency, float duty_cycle, bool inverted = false);

    void SetFrequency(float frequency);
    void SetDutyCycle(float duty_cycle);

    void SetEnabled(bool enable);
};