#pragma once

#include <event/Event.hpp>
#include <util/Debounce.hpp>

#include <platform/platform_config.h>
#include <platform/gpio.h>

class GPIOEvent : public Event
{
protected:
    uint32_t events_triggered_mask;

public:
    GPIOEvent(EventSource* source, uint32_t events_triggered_mask);

    inline uint32_t GetEventsTriggeredMask() const
    {
        return events_triggered_mask;
    }
};

class GPIODevice : public IRQSource
{
public:
    typedef GPIOEvent EventType;

protected:
    static GPIODevice* instances[MCU_NUM_GPIO];

    uint8_t gpio_pin;
    uint32_t event_mask;

    virtual void EnableImpl() override;
    virtual void DisableImpl() override;

    virtual void HandleIRQ(uint32_t events_triggered_mask) override;

private:
    static void gpio_callback(uint8_t gpio_pin, uint32_t events_triggered_mask, void* context);

public:
    GPIODevice(uint8_t gpio_pin, mcu_gpio_pull_t pull, uint32_t event_mask);
    virtual ~GPIODevice();

    inline bool IsHigh() const
    {
        return mcu_gpio_get(gpio_pin);
    }
    inline bool IsLow() const
    {
        return !mcu_gpio_get(gpio_pin);
    }

    virtual bool IsActivated() const;
};

class GPIODeviceDebounce : public GPIODevice
{
protected:
    Debounce debouncer;

    virtual void HandleIRQ(uint32_t events_triggered_mask) override;

public:
    GPIODeviceDebounce(uint8_t gpio_pin, mcu_gpio_pull_t pull, uint32_t event_mask, uint32_t debounce_ms);
    virtual ~GPIODeviceDebounce() = default;
};