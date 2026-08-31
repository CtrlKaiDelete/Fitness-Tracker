#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t IsATtySyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        int FileDescriptor = static_cast<int>(Arg0);
        return this->IsStandardDescriptor(FileDescriptor) ? 1 : this->Fail(EBADF);

    };

};

extern "C" int _isatty(int FileDescriptor) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::IsATty,
        static_cast<uintptr_t>(FileDescriptor)
    ));

};
