#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t LinkSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        return this->Fail(ENOSYS);

    };

};

extern "C" int _link(const char* ExistingPath, const char* NewPath) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Link,
        reinterpret_cast<uintptr_t>(ExistingPath),
        reinterpret_cast<uintptr_t>(NewPath)
    ));

};
