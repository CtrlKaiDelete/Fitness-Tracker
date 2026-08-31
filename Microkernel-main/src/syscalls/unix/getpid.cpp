#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <sys/types.h>

namespace Microkernel {

    uintptr_t GetPidSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        return Current == nullptr ? 0 : Current->GetIdentifier();

    };

};

extern "C" pid_t _getpid() {

    return static_cast<pid_t>(Microkernel::InvokeUnixSyscall(Microkernel::UnixSyscallNumber::GetPid));

};
