#pragma once

#include <cstdint>

namespace FitnessTracker {

    struct AccelerationSample {
        int16_t X = 0;
        int16_t Y = 0;
        int16_t Z = 0;
    };

    class Accelerometer {

    public:

        void Start();
        void Update(uint32_t CurrentTick);
        uint8_t ReadRunningSpeed(uint32_t CurrentTick);
        uint8_t GetRunningSpeed() const;
        uint8_t GetCalories() const;

    private:

        bool WriteRegister(uint8_t Register, uint8_t Value);
        bool ReadRegisters(uint8_t Register, uint8_t* Data, uint32_t Size);
        bool ReadAcceleration(AccelerationSample& Sample);
        bool WaitForTransaction();
        bool Transmit(uint8_t Address, const uint8_t* Data, uint32_t Size);
        bool WriteRead(uint8_t Address, const uint8_t* WriteData, uint32_t WriteSize, uint8_t* ReadData, uint32_t ReadSize);

        bool Available = false;
        static constexpr uint32_t StepThresholdMilligravity = 120;
        static constexpr uint32_t ResetThresholdMilligravity = 60;
        static constexpr uint32_t MinimumStepIntervalMilliseconds = 250;
        static constexpr uint32_t StoppedIntervalMilliseconds = 2000;

        int32_t GravityBaseline = 0;
        uint32_t LastStep = 0;
        uint32_t TotalSteps = 0;
        uint8_t Speed = 0;
        bool BaselineReady = false;
        bool AboveThreshold = false;
        bool SeenFirstStep = false;

    };

}
