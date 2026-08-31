#include "Microkernel/include/drivers/sound.h"
#include "Microkernel/include/drivers/gpio.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cstdint>

namespace Microkernel {

    namespace {

        constexpr uintptr_t Pwm0 = 0x4001C000;
        constexpr uintptr_t TaskStop = Pwm0 + 0x004;
        constexpr uintptr_t TaskSequenceStart = Pwm0 + 0x008;
        constexpr uintptr_t StoppedEvent = Pwm0 + 0x104;
        constexpr uintptr_t SequenceEndEvent = Pwm0 + 0x110;
        constexpr uintptr_t Shorts = Pwm0 + 0x200;
        constexpr uintptr_t InterruptClear = Pwm0 + 0x308;
        constexpr uintptr_t Enable = Pwm0 + 0x500;
        constexpr uintptr_t Mode = Pwm0 + 0x504;
        constexpr uintptr_t CounterTop = Pwm0 + 0x508;
        constexpr uintptr_t Prescaler = Pwm0 + 0x50C;
        constexpr uintptr_t Decoder = Pwm0 + 0x510;
        constexpr uintptr_t Loop = Pwm0 + 0x514;
        constexpr uintptr_t SequencePointer = Pwm0 + 0x520;
        constexpr uintptr_t SequenceCount = Pwm0 + 0x524;
        constexpr uintptr_t SequenceRefresh = Pwm0 + 0x528;
        constexpr uintptr_t SequenceEndDelay = Pwm0 + 0x52C;
        constexpr uintptr_t OutputSelect0 = Pwm0 + 0x560;
        constexpr uintptr_t OutputSelect1 = Pwm0 + 0x564;
        constexpr uintptr_t OutputSelect2 = Pwm0 + 0x568;
        constexpr uintptr_t OutputSelect3 = Pwm0 + 0x56C;

        constexpr uint32_t SpeakerPort = 0;
        constexpr uint32_t SpeakerPin = 0;
        constexpr uint32_t PwmClock = 1000000;
        constexpr uint32_t DisconnectedOutput = 0xFFFFFFFF;

    };

    void SoundDriver::OnStart() {

        auto& GPIODriver = this->Kernel->DeviceManager.LoadDriver<GPIO>();
        GPIODriver.SetOutput(SpeakerPort, SpeakerPin);
        GPIODriver.WritePin(SpeakerPort, SpeakerPin, GPIOLow);

        *reinterpret_cast<volatile uint32_t*>(TaskStop) = 1;
        *reinterpret_cast<volatile uint32_t*>(Enable) = 0;
        *reinterpret_cast<volatile uint32_t*>(StoppedEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(SequenceEndEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(InterruptClear) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(Shorts) = 0;
        *reinterpret_cast<volatile uint32_t*>(Mode) = 0;
        *reinterpret_cast<volatile uint32_t*>(Prescaler) = 4;
        *reinterpret_cast<volatile uint32_t*>(Decoder) = 0;
        *reinterpret_cast<volatile uint32_t*>(Loop) = 0;
        *reinterpret_cast<volatile uint32_t*>(OutputSelect0) = SpeakerPin;
        *reinterpret_cast<volatile uint32_t*>(OutputSelect1) = DisconnectedOutput;
        *reinterpret_cast<volatile uint32_t*>(OutputSelect2) = DisconnectedOutput;
        *reinterpret_cast<volatile uint32_t*>(OutputSelect3) = DisconnectedOutput;

        this->Playing = false;

    };

    bool SoundDriver::PlayTone(uint32_t Frequency, float Volume) {

        DriverWatchdogScope Watchdog(*this);

        if(Frequency < this->MinimumFrequency || Frequency > this->MaximumFrequency
            || Volume < 0.0f || Volume > 1.0f) {

            return false;

        };

        if(Volume == 0.0f) {

            this->StopHardware();
            return true;

        };

        uint32_t Top = PwmClock / Frequency;
        uint32_t Compare = static_cast<uint32_t>(static_cast<float>(Top) * Volume * 0.5f);
        if(Compare == 0) {

            Compare = 1;

        };

        if(this->Playing) {

            // EasyDMA must be finished with the sequence before its backing
            // memory is changed. The PWM continues producing the last loaded
            // value while we wait and while the next value is loaded.
            while(*reinterpret_cast<volatile uint32_t*>(SequenceEndEvent) == 0) {};

        };

        this->SequenceValue = static_cast<uint16_t>(Compare);
        __asm__ volatile("dmb" ::: "memory");

        if(this->Playing) {

            *reinterpret_cast<volatile uint32_t*>(CounterTop) = Top;
            *reinterpret_cast<volatile uint32_t*>(SequenceEndEvent) = 0;
            *reinterpret_cast<volatile uint32_t*>(TaskSequenceStart) = 1;
            return true;

        };

        this->StopHardware();
        *reinterpret_cast<volatile uint32_t*>(CounterTop) = Top;
        *reinterpret_cast<volatile uint32_t*>(SequencePointer) = reinterpret_cast<uintptr_t>(&this->SequenceValue);
        *reinterpret_cast<volatile uint32_t*>(SequenceCount) = 1;
        *reinterpret_cast<volatile uint32_t*>(SequenceRefresh) = 0;
        *reinterpret_cast<volatile uint32_t*>(SequenceEndDelay) = 0;
        *reinterpret_cast<volatile uint32_t*>(SequenceEndEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(Shorts) = 0;
        *reinterpret_cast<volatile uint32_t*>(Enable) = 1;
        *reinterpret_cast<volatile uint32_t*>(TaskSequenceStart) = 1;

        this->Playing = true;
        return true;

    };

    void SoundDriver::StopHardware(bool WaitForStop) {

        bool WasPlaying = this->Playing;
        *reinterpret_cast<volatile uint32_t*>(StoppedEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(Shorts) = 0;
        *reinterpret_cast<volatile uint32_t*>(TaskStop) = 1;

        if(WaitForStop && WasPlaying) {

            while(*reinterpret_cast<volatile uint32_t*>(StoppedEvent) == 0) {};

        };

        *reinterpret_cast<volatile uint32_t*>(Enable) = 0;
        *reinterpret_cast<volatile uint32_t*>(StoppedEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(SequenceEndEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(0x5000050C) = 1u << SpeakerPin;
        this->Playing = false;

    };

    void SoundDriver::Stop() {

        DriverWatchdogScope Watchdog(*this);
        this->StopHardware();

    };

    bool SoundDriver::IsPlaying() const {

        return this->Playing;

    };

    void SoundDriver::OnStop() {

        this->StopHardware();
        *reinterpret_cast<volatile uint32_t*>(OutputSelect0) = DisconnectedOutput;

    };

    void SoundDriver::OnPanic() {

        this->StopHardware(false);
        *reinterpret_cast<volatile uint32_t*>(OutputSelect0) = DisconnectedOutput;

    };

};
