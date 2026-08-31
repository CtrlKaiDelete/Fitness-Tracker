#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t UnlinkSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        char Path[64] = {};
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        int Error = 0;
        return this->Kernel->Filesystem.Remove(Path, Error) ? 0 : this->Fail(Error);

    };

};

extern "C" int _unlink(const char* Path) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Unlink,
        reinterpret_cast<uintptr_t>(Path)
    ));

};
