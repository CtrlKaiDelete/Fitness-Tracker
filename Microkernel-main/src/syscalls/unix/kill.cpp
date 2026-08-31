#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t KillSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        return this->Fail(ENOSYS);

    };

};

extern "C" int _kill(pid_t Process, int Signal) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Kill,
        static_cast<uintptr_t>(Process),
        static_cast<uintptr_t>(Signal)
    ));

};
