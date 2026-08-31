#pragma once

#include "Microkernel/include/base/object.h"

#include <cstdint>

namespace Microkernel {

    class Interrupt : public Object {

    public:

        Interrupt() = default;
        virtual ~Interrupt() = default;

        virtual void Execute() = 0;
        uint32_t InterruptNumber = 0;

    };

};
