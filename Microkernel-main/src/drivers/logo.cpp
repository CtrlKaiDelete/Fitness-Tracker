#include "Microkernel/include/drivers/logo.h"
#include "Microkernel/include/drivers/gpio.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cstdint>

#define DEMCR 0xE000EDFC
#define DWT_CTRL 0xE0001000
#define DWT_CYCCNT 0xE0001004

namespace Microkernel {

    void LogoDriver::OnStart() {

        *reinterpret_cast<volatile uint32_t*>(DEMCR) |= 1u << 24;
        *reinterpret_cast<volatile uint32_t*>(DWT_CYCCNT) = 0;
        *reinterpret_cast<volatile uint32_t*>(DWT_CTRL) |= 1u;

        this->Calibrate();

    };

    void LogoDriver::Calibrate() {

        DriverWatchdogScope Watchdog(*this);
        this->Baseline = 0;

        for(uint32_t Sample = 0; Sample < this->CalibrationSamples; Sample++) {

            uint32_t ChargeTime = this->ReadChargeTime();
            if(ChargeTime > this->Baseline) {

                this->Baseline = ChargeTime;

            };

        };

        this->Threshold = this->Baseline + (this->Baseline / 2) + 32;
        if(this->Threshold > this->MaximumChargeTime) {

            this->Threshold = this->MaximumChargeTime;

        };

        this->SamplesMatchingNewState = 0;
        this->Pressed = false;

    };

    bool LogoDriver::IsPressed() {

        DriverWatchdogScope Watchdog(*this);
        uint32_t ChargeTime = this->ReadChargeTime();
        bool SamplePressed = ChargeTime >= this->Threshold;

        if(SamplePressed == this->Pressed) {

            this->SamplesMatchingNewState = 0;

        } else {

            this->SamplesMatchingNewState++;
            if(this->SamplesMatchingNewState >= this->PressSamples) {

                this->Pressed = SamplePressed;
                this->SamplesMatchingNewState = 0;

            };

        };

        return this->Pressed;

    };

    uint32_t LogoDriver::ReadChargeTime() {

        auto& GPIODriver = this->Kernel->DeviceManager.LoadDriver<GPIO>();
        volatile uint32_t* CycleCounter = reinterpret_cast<volatile uint32_t*>(DWT_CYCCNT);

        uint32_t InterruptState = 0;
        __asm__ volatile("mrs %0, primask" : "=r"(InterruptState));
        __asm__ volatile("cpsid i" ::: "memory");

        GPIODriver.SetOutput(this->LogoPort, this->LogoPin);
        GPIODriver.WritePin(this->LogoPort, this->LogoPin, GPIOLow);

        for(int Cycle = 0; Cycle < 32; Cycle++) {

            __asm__ volatile("nop");

        };

        uint32_t Start = *CycleCounter;
        GPIODriver.SetInput(this->LogoPort, this->LogoPin);

        uint32_t ChargeTime = 0;
        while(GPIODriver.ReadPin(this->LogoPort, this->LogoPin) == GPIOLow) {

            ChargeTime = *CycleCounter - Start;
            if(ChargeTime >= this->MaximumChargeTime) {

                ChargeTime = this->MaximumChargeTime;
                break;

            };

        };

        if(InterruptState == 0) {

            __asm__ volatile("cpsie i" ::: "memory");

        };

        return ChargeTime;

    };

};
