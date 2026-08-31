#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t ChownSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        char Path[64] = {};
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        int Error = 0;
        return this->Kernel->Filesystem.ChangeOwner(Path, static_cast<uint16_t>(Arg1), Error) ?
            0 : this->Fail(Error);

    };

};

extern "C" int _chown(const char* Path, uid_t Owner, gid_t Group) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Chown,
        reinterpret_cast<uintptr_t>(Path),
        static_cast<uintptr_t>(Owner),
        static_cast<uintptr_t>(Group)
    ));

};
