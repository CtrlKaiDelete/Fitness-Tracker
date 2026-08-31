#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    uintptr_t ExitSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr) {

            this->Kernel->Panic("KERNEL THREAD EXIT");

        };

        Current->Exit(static_cast<int>(Arg0));
        this->Kernel->Scheduler.Yield();
        return 0;

    };

};

extern "C" [[noreturn]] void _exit(int Status) {

    Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Exit,
        static_cast<uintptr_t>(Status)
    );
    Microkernel::Kernel::New()->Panic("PROCESS EXIT");

};
