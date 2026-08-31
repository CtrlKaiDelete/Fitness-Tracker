#include "Microkernel/include/drivers/gpio.h"

#include <cstdint>

namespace Microkernel {

    static constexpr uintptr_t Port0 = 0x50000000;
    static constexpr uintptr_t Port1 = 0x50000300;

    void GPIO::WritePin(int Port, int Pin, PinSetting Setting) {

        DriverWatchdogScope Watchdog(*this);
        if((Port > 1 || Port < 0) || Pin < 0) {

            return;

        };

        if((Port == 0 && Pin > 31) || (Port == 1 && Pin > 9)) {

            return;

        };

        uintptr_t Base = Port == 0 ? Port0 : Port1;

        volatile uint32_t* Set = reinterpret_cast<volatile uint32_t*>(Base + 0x508);
        volatile uint32_t* Clear = reinterpret_cast<volatile uint32_t*>(Base + 0x50C);
        uint32_t Mask = 1u << Pin;

        if(Setting == GPIOHigh) {

            *Set = Mask;

        } else {

            *Clear = Mask;

        };

    };

    void GPIO::WritePins(int Port, uint32_t HighMask, uint32_t LowMask) {

        DriverWatchdogScope Watchdog(*this);
        if(Port < 0 || Port > 1) {

            return;

        };

        uint32_t ValidMask = Port == 0 ? 0xFFFFFFFF : 0x000003FF;
        HighMask &= ValidMask;
        LowMask &= ValidMask;

        uintptr_t Base = Port == 0 ? Port0 : Port1;
        volatile uint32_t* Set = reinterpret_cast<volatile uint32_t*>(Base + 0x508);
        volatile uint32_t* Clear = reinterpret_cast<volatile uint32_t*>(Base + 0x50C);

        *Set = HighMask;
        *Clear = LowMask;

    };

    int GPIO::ReadPin(int Port, int Pin) {

        DriverWatchdogScope Watchdog(*this);
        if(Port < 0 || Port > 1 || Pin < 0) {

            return GPIOLow;

        };

        if((Port == 0 && Pin > 31) || (Port == 1 && Pin > 9)) {

            return GPIOLow;

        };

        uintptr_t Base = Port == 0 ? Port0 : Port1;
        volatile uint32_t* Input = reinterpret_cast<volatile uint32_t*>(Base + 0x510);

        uint32_t Mask = 1u << Pin;
        return (*Input & Mask) ? GPIOHigh : GPIOLow;

    };

    void GPIO::SetInput(int Port, int Pin) {

        DriverWatchdogScope Watchdog(*this);
        if(Port < 0 || Port > 1 || Pin < 0) {

            return;

        };

        if((Port == 0 && Pin > 31) || (Port == 1 && Pin > 9)) {

            return;

        };

        uintptr_t Base = Port == 0 ? Port0 : Port1;
        volatile uint32_t* DirectionClear = reinterpret_cast<volatile uint32_t*>(Base + 0x51C);
        volatile uint32_t* PinConfiguration = reinterpret_cast<volatile uint32_t*>(Base + 0x700 + (Pin * 4));

        *DirectionClear = 1u << Pin;
        *PinConfiguration &= ~(1u << 1);

    };

    void GPIO::SetOutput(int Port, int Pin) {

        DriverWatchdogScope Watchdog(*this);
        if(Port < 0 || Port > 1 || Pin < 0) {

            return;

        };

        if((Port == 0 && Pin > 31) || (Port == 1 && Pin > 9)) {

            return;

        };

        uintptr_t Base = Port == 0 ? Port0 : Port1;
        volatile uint32_t* DirectionSet = reinterpret_cast<volatile uint32_t*>(Base + 0x518);

        *DirectionSet = 1u << Pin;

    };

};
