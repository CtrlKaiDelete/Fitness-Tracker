#include "Microkernel/include/interrupts/timerinterrupt.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    TimerInterrupt::TimerInterrupt() {

        this->InterruptNumber = 8;

    };

    void TimerInterrupt::Execute() {

        Kernel::New()->InterruptTimer.HandleInterrupt();

    };

};