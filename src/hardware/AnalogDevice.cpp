#include <hardware/AnalogDevice.hpp>
#include <platform/adc.h>

AnalogEvent::AnalogEvent(EventSource* source, uint8_t adc_pin, uint32_t adc_value)
    : Event(source), adc_value(adc_value)
{
}

AnalogDevice::AnalogDevice(uint8_t adc_pin, uint32_t adc_max_activation)
    : EventSource(), adc_pin(adc_pin), adc_max_activation(adc_max_activation)
{
    mcu_adc_init(adc_pin);
}

AnalogRepeatingDevice::AnalogRepeatingDevice(uint8_t adc_pin, uint32_t adc_max_activation, RepeatingTimer& repeat_timer, uint32_t window_ms, uint32_t adc_spacing_break)
    : AnalogDevice(adc_pin, adc_max_activation), repeat_timer(repeat_timer)
{
    init_data = new _InitData{break_detection_timer, latched_adc_value};
    std::ignore = AddAction([](const Event* ev, void* ptr){
        _InitData* data = (_InitData*)ptr;
        AnalogEvent* event = ev->GetEventAsType<AnalogEvent>();

        data->latched_adc = event->GetADCValue();
        data->break_detection_timer.Start(interval_ms);
    }, init_data);

    timer_data = new _TimerData{repeat_timer, elapsed_ms, window_ms, adc_spacing_break, latched_adc_value, adc_pin};
    // gets called every interval_ms milliseconds (defined in header file) until the ADC value changes more than the delta (adc_spacing_break)
    std::ignore = break_detection_timer.AddAction([](const Event* ev, void* ptr){
        _TimerData* data = (_TimerData*)ptr;
        RepeatingTimer* self = ev->GetSourceAsType<RepeatingTimer>();

        uint32_t adc_v_then = data->latched_adc;
        uint32_t adc_v_now = mcu_adc_read(data->adc_pin);

        uint32_t adc_bound_high = adc_v_then + data->adc_spacing_break;
        uint32_t adc_bound_low = adc_v_then - data->adc_spacing_break;

        // if the new adc value has a delta of 50 (device no longer active), end everything
        if (adc_v_now > adc_bound_high || adc_v_now < adc_bound_low)
        {
            self->End(); // should be called when the device goes inactive
            data->action_repeat_timer.End();
            data->elapsed_ms = 0;
            return;
        }
        
        if (data->action_repeat_timer.IsActive())
            return;

        // if enough time has passed without a changed adc value, begin the repeat until the adc value changes
        data->elapsed_ms += interval_ms;
        if (data->elapsed_ms >= data->wait_window_ms)
        {
            data->action_repeat_timer.Start();
        }

    }, timer_data);
}

AnalogRepeatingDevice::~AnalogRepeatingDevice()
{
    delete init_data;
    delete timer_data;
}