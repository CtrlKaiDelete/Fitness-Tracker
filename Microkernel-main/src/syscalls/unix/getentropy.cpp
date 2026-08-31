#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>
#include <cstddef>

namespace Microkernel {

    uintptr_t GetEntropySyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        return this->Fail(ENOSYS);

    };

};

extern "C" int _getentropy(void* Buffer, size_t Size) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::GetEntropy,
        reinterpret_cast<uintptr_t>(Buffer),
        static_cast<uintptr_t>(Size)
    ));

};
