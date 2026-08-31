#pragma once

#include "Microkernel/include/base/object.h"

#include <cstdint>

namespace Microkernel {

    class Timer : public Object {

    public:

        Timer();
        virtual ~Timer() = default;

        void Start();
        void Stop();
        uint64_t GetTicks();
        uint64_t GetMicroseconds();
        uint64_t GetMilliseconds();
        uint64_t GetSeconds();

        void HandleInterrupt();

    private:

        uint64_t Ticks = 0;

    };

};
