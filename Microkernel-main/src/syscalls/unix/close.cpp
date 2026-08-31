#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t CloseSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        int FileDescriptor = static_cast<int>(Arg0);
        if(this->IsStandardDescriptor(FileDescriptor)) {

            return 0;

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        return Current != nullptr && Current->GetHandles().Close(FileDescriptor) ? 0 : this->Fail(EBADF);

    };

};

extern "C" int _close(int FileDescriptor) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Close,
        static_cast<uintptr_t>(FileDescriptor)
    ));

};
