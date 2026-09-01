#include "Audio.hpp"

#include "Hardware.hpp"

namespace FitnessTracker {

    namespace {
        uint16_t ToneDuty = 0;
    }

    void StopTone() {

        Write(Pwm0StoppedEvent, 0);
        Write(Pwm0Stop, 1);
        Write(Pwm0Enable, 0);
        Write(Pwm0StoppedEvent, 0);
        Write(Pwm0SequenceEndEvent, 0);
        Write(Port0OutputClear, 1u << SpeakerPin);

    }

    void StartTone(uint32_t Frequency) {

        uint32_t Top = PwmClock / Frequency;
        ToneDuty = static_cast<uint16_t>(Top / 4);
        if(ToneDuty == 0) {
            ToneDuty = 1;
        }

        Write(Port0DirectionSet, 1u << SpeakerPin);
        StopTone();
        Write(Pwm0InterruptClear, 0xFFFFFFFF);
        Write(Pwm0Shorts, 0);
        Write(Pwm0Mode, 0);
        Write(Pwm0Prescaler, 4);
        Write(Pwm0Decoder, 0);
        Write(Pwm0Loop, 0);
        Write(Pwm0CounterTop, Top);
        Write(Pwm0SequencePointer, reinterpret_cast<uintptr_t>(&ToneDuty));
        Write(Pwm0SequenceCount, 1);
        Write(Pwm0SequenceRefresh, 0);
        Write(Pwm0SequenceEndDelay, 0);
        Write(Pwm0OutputSelect0, SpeakerPin);
        Write(Pwm0OutputSelect1, DisconnectedOutput);
        Write(Pwm0OutputSelect2, DisconnectedOutput);
        Write(Pwm0OutputSelect3, DisconnectedOutput);
        Write(Pwm0SequenceEndEvent, 0);
        Write(Pwm0Enable, 1);
        Write(Pwm0SequenceStart, 1);

    }

}
