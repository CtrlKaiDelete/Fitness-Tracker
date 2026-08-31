#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>
#include <sys/time.h>

namespace Microkernel {

    uintptr_t GetTimeOfDaySyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        return this->Fail(ENOSYS);

    };

};

extern "C" int _gettimeofday(struct timeval* Time, void* Timezone) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::GetTimeOfDay,
        reinterpret_cast<uintptr_t>(Time),
        reinterpret_cast<uintptr_t>(Timezone)
    ));

};
