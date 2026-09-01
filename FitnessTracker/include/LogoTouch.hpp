#pragma once

#include <cstdint>

namespace FitnessTracker {

    class LogoTouch {

    public:

        void Start();
        bool IsPressed();

    private:

        void Calibrate();
        uint32_t ReadChargeTime();

        static constexpr uint32_t CalibrationSamples = 16;
        static constexpr uint32_t MaximumChargeTime = 64000;
        static constexpr uint32_t PressSamples = 8;

        uint32_t Baseline = 0;
        uint32_t Threshold = 0;
        uint32_t SamplesMatchingNewState = 0;
        bool Pressed = false;

    };

}
