#include <hardware/SerialUSB.hpp>

// WIP --- file has many errors

void tud_mount_cb(void)
{
    if (serial_usb_detector_instance)
    {
        serial_usb_detector_instance->usb_stat = UP;
        if (serial_usb_detector_instance->IsEnabled())
        {
            Event* ev = new USBUpdateEvent(serial_usb_detector_instance, USBUpdateEvent::CONNECTED);
            serial_usb_detector_instance->ProcessImmediateActions(ev);
            Event::AddToEventQueue(&ev)
        }
    }
}

void tud_umount_cb(void)
{
    if (serial_usb_detector_instance)
    {
        serial_usb_detector_instance->usb_stat = DOWN;
        if (serial_usb_detector_instance->IsEnabled())
        {
            Event* ev = new USBUpdateEvent(serial_usb_detector_instance, USBUpdateEvent::DISCONNECTED);
            serial_usb_detector_instance->ProcessImmediateActions(ev);
            Event::AddToEventQueue(&ev)
        }
    }
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    if (serial_usb_detector_instance)
    {
        serial_usb_detector_instance->usb_stat = DOWN;
        if (serial_usb_detector_instance->IsEnabled())
        {
            Event* ev = new USBUpdateEvent(serial_usb_detector_instance, USBUpdateEvent::SUSPENDED);  
            serial_usb_detector_instance->ProcessImmediateActions(ev); 
            Event::AddToEventQueue(&ev)
        }
    }
}

void tud_resume_cb(void)
{
    if (serial_usb_detector_instance)
    {
        serial_usb_detector_instance->usb_stat = UP; // CONNECTED, as RESUMED is only for instantaneous changes.
        if (serial_usb_detector_instance->IsEnabled())
        {
            Event* ev = new USBUpdateEvent(serial_usb_detector_instance, USBUpdateEvent::RESUMED);
            serial_usb_detector_instance->ProcessImmediateActions(ev);
            Event::AddToEventQueue(&ev)
        }
    }
}