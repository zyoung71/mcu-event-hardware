#include "platform/gpio.h"
#include "platform/platform_config.h"
#include "platform/util/queue.h"
#include <hardware/GPIODevice.hpp>

GPIOEvent::GPIOEvent(EventSource* source, uint32_t events_triggered_mask)
    : Event(source), events_triggered_mask(events_triggered_mask)
{
}

GPIODevice* GPIODevice::instances[MCU_NUM_GPIO] = {nullptr};

void GPIODevice::gpio_callback(uint8_t gpio_pin, uint32_t events_triggered_mask, void* context)
{
    (void)context;
    
    if (instances[gpio_pin])
    {
        instances[gpio_pin]->HandleIRQ(events_triggered_mask);
    }
}

GPIODevice::GPIODevice(uint8_t gpio_pin, mcu_gpio_pull_t pull, uint32_t event_mask)
    : IRQSource(), gpio_pin(gpio_pin), event_mask(event_mask)
{
    gpio_pin = gpio_pin > 30 ? 30 : gpio_pin;
    instances[gpio_pin] = this;
    
    mcu_gpio_init(gpio_pin);
    mcu_gpio_dir(gpio_pin, MCU_GPIO_INPUT);
    mcu_gpio_pull(gpio_pin, pull);

    mcu_gpio_irq_set(gpio_pin, event_mask, &gpio_callback, nullptr);
    mcu_gpio_irq_enable(gpio_pin, true);
}

GPIODevice::~GPIODevice()
{
    instances[gpio_pin] = nullptr;
}

void GPIODevice::EnableImpl()
{
    mcu_gpio_irq_enable(gpio_pin, true);
}

void GPIODevice::DisableImpl()
{
    mcu_gpio_irq_enable(gpio_pin, false);
}

void GPIODevice::HandleIRQ(uint32_t events_triggered_mask)
{
    if (event_mask & events_triggered_mask)
    {
        Event* ev = new GPIOEvent(this, events_triggered_mask);
        ProcessImmediateActions(ev);
        mcu_queue_try_push(Event::event_queue, &ev);
    }
}

bool GPIODevice::IsActivated() const
{
    return mcu_gpio_get(gpio_pin);
}

GPIODeviceDebounce::GPIODeviceDebounce(uint8_t gpio_pin, mcu_gpio_pull_t pull, uint32_t event_mask, uint32_t debounce_ms)
    : GPIODevice(gpio_pin, pull, event_mask), debouncer(debounce_ms)
{
}

void GPIODeviceDebounce::HandleIRQ(uint32_t events_triggered_mask)
{
    if (event_mask & events_triggered_mask)
    {
        if (debouncer.Allow())
        {
            Event* ev = new GPIOEvent(this, events_triggered_mask); 
            ProcessImmediateActions(ev);  
            mcu_queue_try_push(Event::event_queue, &ev);
        }
    }
}