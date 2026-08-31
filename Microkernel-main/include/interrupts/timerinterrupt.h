#pragma once

#include "Microkernel/include/interrupts/interrupt.h"

namespace Microkernel {

    class TimerInterrupt : public Interrupt {

    public:

        TimerInterrupt();
        virtual ~TimerInterrupt() = default;

        void Execute() override;

    };

};
