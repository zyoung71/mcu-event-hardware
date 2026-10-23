if (NOT TARGET mcu-event-hardware)

    add_library(mcu-event-hardware STATIC
        src/event/Event.cpp
        src/event/Command.cpp
        src/event/USBOverrides.cpp
        src/hardware/GPIODevice.cpp
        src/hardware/Timer.cpp
        src/hardware/Button.cpp
        src/hardware/SerialUSB.cpp
        src/hardware/AnalogDevice.cpp
        src/hardware/PulseWidthModulation.cpp
    )
    
    add_subdirectory(lib/mcu-util)
    add_subdirectory(lib/pico-comms)

    target_include_directories(mcu-event-hardware PUBLIC
        include
    )
    
    target_link_libraries(mcu-event-hardware
        mcu-platform-interface
        mcu-util
        pico-comms
    )
    
endif()