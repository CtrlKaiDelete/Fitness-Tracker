#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstdint>

namespace Microkernel {

    class SoundDriver : public Driver {

    public:

        SoundDriver() = default;
        virtual ~SoundDriver() = default;

        bool PlayTone(uint32_t Frequency, float Volume = 0.5f);
        void Stop();
        [[nodiscard]] bool IsPlaying() const;

    protected:

        void OnStart() override;
        void OnStop() override;
        void OnPanic() override;

    private:

        void StopHardware(bool WaitForStop = true);

        static constexpr uint32_t MinimumFrequency = 31;
        static constexpr uint32_t MaximumFrequency = 20000;

        alignas(4) uint16_t SequenceValue = 0;
        bool Playing = false;

    };

};
