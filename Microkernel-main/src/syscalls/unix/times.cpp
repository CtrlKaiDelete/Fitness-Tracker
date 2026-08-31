#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>
#include <sys/times.h>

namespace Microkernel {

    uintptr_t TimesSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        return this->Fail(ENOSYS);

    };

};

extern "C" clock_t _times(struct tms* Times) {

    return static_cast<clock_t>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Times,
        reinterpret_cast<uintptr_t>(Times)
    ));

};
