#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <cstddef>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t WriteSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        int FileDescriptor = static_cast<int>(Arg0);
        auto Buffer = reinterpret_cast<const void*>(Arg1);
        size_t Size = static_cast<size_t>(Arg2);

        if(FileDescriptor == 1 || FileDescriptor == 2) {

            if(!this->ValidateRead(Buffer, Size)) {

                return this->Fail(EFAULT);

            };

            return Size;

        };

        if(this->IsStandardDescriptor(FileDescriptor)) {

            return this->Fail(EBADF);

        };

        if(!this->ValidateRead(Buffer, Size)) {

            return this->Fail(EFAULT);

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto File = Current == nullptr ? nullptr : static_cast<OpenFile*>(Current->GetHandles().Get(
            FileDescriptor,
            Capability::Write,
            ResourceType::File
        ));
        if(File == nullptr) {

            return this->Fail(EBADF);

        };

        int Error = 0;
        ptrdiff_t Result = File->Write(Buffer, Size, Error);
        return Result < 0 ? this->Fail(Error) : static_cast<uintptr_t>(Result);

    };

};

extern "C" ssize_t _write(int FileDescriptor, const void* Buffer, size_t Size) {

    return static_cast<ssize_t>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Write,
        static_cast<uintptr_t>(FileDescriptor),
        reinterpret_cast<uintptr_t>(Buffer),
        static_cast<uintptr_t>(Size)
    ));

};
