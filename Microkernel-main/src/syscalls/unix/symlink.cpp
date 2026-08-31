#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t SymlinkSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        char Target[64] = {};
        char Path[64] = {};
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Target, sizeof(Target)) ||
            !this->CopyUserString(reinterpret_cast<const char*>(Arg1), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        if(this->Kernel->Filesystem.Resolve(Path, false) != nullptr) {

            return this->Fail(EEXIST);

        };

        return this->Kernel->Filesystem.CreateSymbolicLink(Target, Path) ? 0 : this->Fail(ENOENT);

    };

};

extern "C" int _symlink(const char* Target, const char* LinkPath) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Symlink,
        reinterpret_cast<uintptr_t>(Target),
        reinterpret_cast<uintptr_t>(LinkPath)
    ));

};
