#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstdint>

namespace Microkernel {

    class LogoDriver : public Driver {

    public:

        LogoDriver() = default;
        virtual ~LogoDriver() = default;

        bool IsPressed();
        void Calibrate();

    protected:

        void OnStart() override;
        uint32_t ReadChargeTime();

        static constexpr int LogoPort = 1;
        static constexpr int LogoPin = 4;
        static constexpr uint32_t CalibrationSamples = 16;
        static constexpr uint32_t MaximumChargeTime = 64000;
        static constexpr uint32_t PressSamples = 3;

        uint32_t Baseline = 0;
        uint32_t Threshold = 0;
        uint32_t SamplesMatchingNewState = 0;
        bool Pressed = false;

    };

};
