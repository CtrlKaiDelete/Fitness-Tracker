#include "LogoTouch.hpp"

#include "Hardware.hpp"

namespace FitnessTracker {

    void LogoTouch::Start() {

        Write(DebugExceptionControl, Read(DebugExceptionControl) | (1u << 24));
        Write(CycleCounter, 0);
        Write(CycleCounterControl, Read(CycleCounterControl) | 1u);
        Calibrate();

    }

    bool LogoTouch::IsPressed() {

        uint32_t ChargeTime = ReadChargeTime();
        bool SamplePressed = ChargeTime >= Threshold;
        if(SamplePressed == Pressed) {
            SamplesMatchingNewState = 0;
        } else {
            SamplesMatchingNewState++;
            if(SamplesMatchingNewState >= PressSamples) {
                Pressed = SamplePressed;
                SamplesMatchingNewState = 0;
            }
        }

        return Pressed;

    }

    void LogoTouch::Calibrate() {

        Baseline = 0;
        for(uint32_t Sample = 0; Sample < CalibrationSamples; Sample++) {
            uint32_t ChargeTime = ReadChargeTime();
            if(ChargeTime > Baseline) {
                Baseline = ChargeTime;
            }
        }

        Threshold = Baseline + (Baseline / 2) + 32;
        if(Baseline >= MaximumChargeTime - 64) {
            Threshold = MaximumChargeTime + 1;
        } else if(Threshold > MaximumChargeTime) {
            Threshold = MaximumChargeTime + 1;
        }

        SamplesMatchingNewState = 0;
        Pressed = false;

    }

    uint32_t LogoTouch::ReadChargeTime() {

        Write(Port1DirectionSet, 1u << LogoPin);
        Write(Port1OutputClear, 1u << LogoPin);
        for(int Cycle = 0; Cycle < 32; Cycle++) {
            __asm__ volatile("nop");
        }

        uint32_t StartCycle = Read(CycleCounter);
        Write(Port1DirectionClear, 1u << LogoPin);
        Write(Port1PinConfigLogo, Read(Port1PinConfigLogo) & ~(1u << 1));

        uint32_t ChargeTime = 0;
        while((Read(Port1Input) & (1u << LogoPin)) == 0) {
            ChargeTime = Read(CycleCounter) - StartCycle;
            if(ChargeTime >= MaximumChargeTime) {
                ChargeTime = MaximumChargeTime;
                break;
            }
        }

        return ChargeTime;

    }

}
