#pragma once

#include "Microkernel/include/interrupts/interrupt.h"

namespace Microkernel {

    class DisplayDriver;
    class DisplayInterrupt : public Interrupt {

    public:

        DisplayInterrupt();
        virtual ~DisplayInterrupt() = default;

        void Execute() override;
        DisplayDriver* Display = nullptr;

    };

};
