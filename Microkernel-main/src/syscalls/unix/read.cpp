#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <cstddef>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t ReadSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        int FileDescriptor = static_cast<int>(Arg0);
        auto Buffer = reinterpret_cast<void*>(Arg1);
        size_t Size = static_cast<size_t>(Arg2);

        if(FileDescriptor == 0) {

            if(!this->ValidateWrite(Buffer, Size)) {

                return this->Fail(EFAULT);

            };

            return 0;

        };

        if(this->IsStandardDescriptor(FileDescriptor)) {

            return this->Fail(EBADF);

        };

        if(!this->ValidateWrite(Buffer, Size)) {

            return this->Fail(EFAULT);

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto File = Current == nullptr ? nullptr : static_cast<OpenFile*>(Current->GetHandles().Get(
            FileDescriptor,
            Capability::Read,
            ResourceType::File
        ));
        if(File == nullptr) {

            return this->Fail(EBADF);

        };

        int Error = 0;
        ptrdiff_t Result = File->Read(Buffer, Size, Error);
        return Result < 0 ? this->Fail(Error) : static_cast<uintptr_t>(Result);

    };

};

extern "C" ssize_t _read(int FileDescriptor, void* Buffer, size_t Size) {

    return static_cast<ssize_t>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Read,
        static_cast<uintptr_t>(FileDescriptor),
        reinterpret_cast<uintptr_t>(Buffer),
        static_cast<uintptr_t>(Size)
    ));

};
