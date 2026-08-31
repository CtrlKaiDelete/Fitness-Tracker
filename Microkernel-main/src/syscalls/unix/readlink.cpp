#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <cstddef>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t ReadLinkSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        char Path[64] = {};
        auto Buffer = reinterpret_cast<char*>(Arg1);
        size_t Size = static_cast<size_t>(Arg2);
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        if(!this->ValidateWrite(Buffer, Size)) {

            return this->Fail(EFAULT);

        };

        const FileNode* Node = this->Kernel->Filesystem.Resolve(Path, false);
        if(Node == nullptr) {

            return this->Fail(ENOENT);

        };

        if(Node->GetType() != FileType::SymbolicLink) {

            return this->Fail(EINVAL);

        };

        return Node->ReadData(Buffer, Size);

    };

};

extern "C" ssize_t _readlink(const char* Path, char* Buffer, size_t Size) {

    return static_cast<ssize_t>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::ReadLink,
        reinterpret_cast<uintptr_t>(Path),
        reinterpret_cast<uintptr_t>(Buffer),
        static_cast<uintptr_t>(Size)
    ));

};
