#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstdint>

namespace Microkernel {

    static constexpr int GPIOLow = 0;
    static constexpr int GPIOHigh = 1;
    typedef int PinSetting;

    class GPIO : public Driver {

    public:

        GPIO() = default;
        virtual ~GPIO() = default;

        void SetInput(int Port, int Pin);
        void SetOutput(int Port, int Pin);

        int ReadPin(int Port, int Pin);
        void WritePin(int Port, int Pin, PinSetting Setting);
        void WritePins(int Port, uint32_t HighMask, uint32_t LowMask);

    };

};
