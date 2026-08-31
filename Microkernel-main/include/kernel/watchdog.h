#pragma once

#include "Microkernel/include/base/object.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    class Driver;
    class DriverWatchdog : public Object {

    public:

        static constexpr uint32_t DefaultTimeoutMicroseconds = 100000;

        bool Initialize(uint32_t TimeoutMicroseconds = DefaultTimeoutMicroseconds);
        void Arm(Driver& Driver);
        void Disarm(Driver& Driver);
        void Stop();
        void HandleInterrupt();

        [[nodiscard]] bool IsArmed() const;
        [[nodiscard]] const Driver* GetActiveDriver() const;

    private:

        void RestartTimer();

        static constexpr std::size_t MaximumDepth = 8;

        Driver* DriverStack[MaximumDepth] = {};
        volatile std::size_t Depth = 0;
        uint32_t Timeout = DefaultTimeoutMicroseconds;
        bool Initialized = false;

    };

};
