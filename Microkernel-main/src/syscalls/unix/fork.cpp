#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t ForkSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        Process* Child = this->Kernel->Processes.ForkCurrent();
        return Child == nullptr ? this->Fail(EAGAIN) : Child->GetIdentifier();

    };

};

extern "C" pid_t _fork() {

    return static_cast<pid_t>(Microkernel::InvokeUnixSyscall(Microkernel::UnixSyscallNumber::Fork));

};
