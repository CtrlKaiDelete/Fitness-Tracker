#include "Accelerometer.hpp"

#include "Hardware.hpp"
#include "Math.hpp"

namespace FitnessTracker {

    void Accelerometer::Start() {

        Write(I2cEnable, 0);
        Write(I2cClockPinSelect, 8);
        Write(I2cDataPinSelect, 16);
        Write(I2cFrequency, 0x06400000);
        Write(I2cEnable, 6);

        uint8_t Identity = 0;
        Available = ReadRegisters(0x0F, &Identity, 1) && Identity == 0x33;
        if(!Available) {
            return;
        }

        Available = WriteRegister(0x20, 0x47) && WriteRegister(0x23, 0x88);

    }

    uint8_t Accelerometer::ReadRunningSpeed(uint32_t CurrentTick) {

        AccelerationSample Sample = {};
        if(!Available || !ReadAcceleration(Sample)) {
            return 0;
        }

        uint32_t Magnitude = GetMagnitude(Sample.X, Sample.Y, Sample.Z);
        if(!BaselineReady) {
            GravityBaseline = static_cast<int32_t>(Magnitude);
            BaselineReady = true;
            return 0;
        }

        int32_t Difference = static_cast<int32_t>(Magnitude) - GravityBaseline;
        GravityBaseline += Difference / 32;
        uint32_t MotionMagnitude = static_cast<uint32_t>(Difference < 0 ? -Difference : Difference);

        if(!AboveThreshold &&
            MotionMagnitude >= StepThresholdMilligravity &&
            CurrentTick - LastStep >= MinimumStepIntervalMilliseconds) {

            if(SeenFirstStep) {
                uint32_t StepInterval = CurrentTick - LastStep;
                if(StepInterval <= StoppedIntervalMilliseconds) {
                    uint32_t InstantSpeed = 18000u / StepInterval;
                    if(InstantSpeed > 99) {
                        InstantSpeed = 99;
                    }

                    Speed = static_cast<uint8_t>(((static_cast<uint16_t>(Speed) * 4u) + InstantSpeed) / 5u);
                }
            } else {
                SeenFirstStep = true;
                Speed = 0;
            }

            LastStep = CurrentTick;
            AboveThreshold = true;

        } else if(AboveThreshold && MotionMagnitude <= ResetThresholdMilligravity) {

            AboveThreshold = false;

        }

        if(CurrentTick - LastStep >= StoppedIntervalMilliseconds) {
            Speed = 0;
        }

        return Speed;

    }

    bool Accelerometer::WriteRegister(uint8_t Register, uint8_t Value) {

        uint8_t Data[2] = {Register, Value};
        return Transmit(0x19, Data, 2);

    }

    bool Accelerometer::ReadRegisters(uint8_t Register, uint8_t* Data, uint32_t Size) {

        uint8_t Address = Size > 1 ? static_cast<uint8_t>(Register | 0x80) : Register;
        return WriteRead(0x19, &Address, 1, Data, Size);

    }

    bool Accelerometer::ReadAcceleration(AccelerationSample& Sample) {

        uint8_t Data[6] = {};
        if(!ReadRegisters(0x28, Data, 6)) {
            return false;
        }

        Sample.X = static_cast<int16_t>(static_cast<uint16_t>(Data[0]) |
            static_cast<uint16_t>(Data[1]) << 8) >> 4;
        Sample.Y = static_cast<int16_t>(static_cast<uint16_t>(Data[2]) |
            static_cast<uint16_t>(Data[3]) << 8) >> 4;
        Sample.Z = static_cast<int16_t>(static_cast<uint16_t>(Data[4]) |
            static_cast<uint16_t>(Data[5]) << 8) >> 4;
        return true;

    }

    bool Accelerometer::WaitForTransaction() {

        for(uint32_t Timeout = 0; Timeout < 1000000; Timeout++) {
            if(Read(I2cStoppedEvent) != 0) {
                return Read(I2cErrorEvent) == 0;
            }

            if(Read(I2cErrorEvent) != 0) {
                Write(I2cStop, 1);
            }
        }

        return false;

    }

    bool Accelerometer::Transmit(uint8_t Address, const uint8_t* Data, uint32_t Size) {

        Write(I2cStoppedEvent, 0);
        Write(I2cErrorEvent, 0);
        Write(I2cErrorSource, 0xFFFFFFFF);
        Write(I2cShortcuts, 1u << 9);
        Write(I2cTransmitPointer, reinterpret_cast<uintptr_t>(Data));
        Write(I2cTransmitMaximum, Size);
        Write(I2cAddress, Address);
        Write(I2cStartTransmit, 1);
        return WaitForTransaction();

    }

    bool Accelerometer::WriteRead(uint8_t Address, const uint8_t* WriteData, uint32_t WriteSize, uint8_t* ReadData, uint32_t ReadSize) {

        Write(I2cStoppedEvent, 0);
        Write(I2cErrorEvent, 0);
        Write(I2cErrorSource, 0xFFFFFFFF);
        Write(I2cShortcuts, (1u << 7) | (1u << 12));
        Write(I2cTransmitPointer, reinterpret_cast<uintptr_t>(WriteData));
        Write(I2cTransmitMaximum, WriteSize);
        Write(I2cReceivePointer, reinterpret_cast<uintptr_t>(ReadData));
        Write(I2cReceiveMaximum, ReadSize);
        Write(I2cAddress, Address);
        Write(I2cStartTransmit, 1);
        return WaitForTransaction();

    }

}
