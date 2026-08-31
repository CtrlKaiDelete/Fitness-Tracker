#include "Microkernel/include/kernel/kernel.h"
#include "Microkernel/include/kernel/syscallmanager.h"
#include "Microkernel/include/libkern/exceptionframe.h"

#include <cstdint>

namespace Microkernel {

    uintptr_t SyscallManager::Execute(uint32_t Number, uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        if(Number >= this->Syscalls.size()) {

            return static_cast<uintptr_t>(-1);

        };

        Syscall* Call = this->Syscalls[Number];
        if(Call == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        return Call->Execute(Arg0, Arg1, Arg2);

    };

    void SyscallManager::HandleExceptionFrame(ExceptionFrame* E) {

        this->CurrentFrame = E;
        uintptr_t Result = this->Execute(E->R0, E->R1, E->R2, E->R3);
        if(this->CurrentFrame == E) {

            E->R0 = Result;

        };

        this->CurrentFrame = nullptr;
        this->CurrentRegisters = nullptr;
        this->CurrentExceptionReturn = 0;

    };

    bool SyscallManager::ReplaceCurrentExceptionFrame(ExceptionFrame* Frame) {

        if(this->CurrentFrame == nullptr || Frame == nullptr) {

            return false;

        };

        this->CurrentFrame = Frame;
        return true;

    };

    const ExceptionFrame* SyscallManager::GetCurrentFrame() const {

        return this->CurrentFrame;

    };

    const uint32_t* SyscallManager::GetCurrentRegisters() const {

        return this->CurrentRegisters;

    };

    uint32_t SyscallManager::GetCurrentExceptionReturn() const {

        return this->CurrentExceptionReturn;

    };

    void SetSyscallContext(
        SyscallManager& Manager,
        const uint32_t* Registers,
        uint32_t ExceptionReturn
    ) {

        Manager.CurrentRegisters = Registers;
        Manager.CurrentExceptionReturn = ExceptionReturn;

    };

};

extern "C" void kernel_svc_dispatch(
    Microkernel::ExceptionFrame* Frame,
    const uint32_t* Registers,
    uint32_t ExceptionReturn
) {

    auto& Manager = Microkernel::Kernel::New()->SyscallManager;
    Microkernel::SetSyscallContext(Manager, Registers, ExceptionReturn);
    Manager.HandleExceptionFrame(Frame);

};
