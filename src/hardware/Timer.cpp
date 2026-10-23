#include "platform/time.h"
#include "platform/util/queue.h"
#include <hardware/Timer.hpp>

TimerEvent::TimerEvent(EventSource* source)
    : Event(source)
{
}

void Timer::End()
{
    if (id >= 0 || is_active)
    {
        mcu_timer_cancel(id);
        is_active = false;
    }
}

void CountdownTimer::timer_callback(void* context)
{
    CountdownTimer* self = (CountdownTimer*)context;

    Event* ev = new TimerEvent(self);
    self->ProcessImmediateActions(ev);
    mcu_queue_try_push(Event::event_queue, &ev);

    self->us_start = 0;
    self->End();
}

void CountdownTimer::Start(uint32_t ms)
{
    if (!is_enabled)
        return;

    if (id >= 0 || is_active)
    {
        End();
        return Start(ms);
    }

    us_begintime = ms * 1000ULL;
    us_start = mcu_get_time_since_boot_us();
    id = mcu_add_timer_once(ms * 1000ULL, &timer_callback, this);
}

void RepeatingTimer::timer_callback(void* context)
{
    RepeatingTimer* self = (RepeatingTimer*)context;

    Event* ev = new TimerEvent(self);
    self->ProcessImmediateActions(ev);
    mcu_queue_try_push(Event::event_queue, &ev);
    
    self->us_start = mcu_get_time_since_boot_us();
}

void RepeatingTimer::Start(uint32_t ms)
{
    if (!is_enabled)
        return;

    if (is_active)
    {
        End();
        return Start(ms);
    }

    us_begintime = ms * 1000ULL;
    us_start = mcu_get_time_since_boot_us();
    id = mcu_add_timer_periodic(ms * 1000ULL, &timer_callback, this);
    is_active = true;
}