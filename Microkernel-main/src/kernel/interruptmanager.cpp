#include "Microkernel/include/kernel/interruptmanager.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    void InterruptManager::HandleInterrupt(uint32_t Number) {

        if(Number >= this->Interrupts.size()) {

            return;

        };

        Interrupt* Handler = this->Interrupts[Number];
        if(Handler == nullptr) {

            return;

        };

        Handler->Execute();

    };

    void InterruptManager::DisableAll() {

        *reinterpret_cast<volatile uint32_t*>(0xE000E180) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(0xE000E184) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(0xE000E280) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(0xE000E284) = 0xFFFFFFFF;

    };

};

extern "C" void kernel_irq_dispatch(uint32_t Number) {

    Microkernel::Kernel::New()->InterruptManager.HandleInterrupt(Number);

};
