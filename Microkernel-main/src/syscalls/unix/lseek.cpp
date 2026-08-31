#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t LseekSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        int FileDescriptor = static_cast<int>(Arg0);
        if(this->IsStandardDescriptor(FileDescriptor)) {

            return this->Fail(ESPIPE);

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto File = Current == nullptr ? nullptr : static_cast<OpenFile*>(Current->GetHandles().Get(
            FileDescriptor,
            Capability::Read,
            ResourceType::File
        ));
        if(File == nullptr) {

            File = Current == nullptr ? nullptr : static_cast<OpenFile*>(Current->GetHandles().Get(
                FileDescriptor,
                Capability::Write,
                ResourceType::File
            ));

        };

        if(File == nullptr) {

            return this->Fail(EBADF);

        };

        int Error = 0;
        ptrdiff_t Result = File->Seek(static_cast<ptrdiff_t>(Arg1), static_cast<int>(Arg2), Error);
        return Result < 0 ? this->Fail(Error) : static_cast<uintptr_t>(Result);

    };

};

extern "C" off_t _lseek(int FileDescriptor, off_t Offset, int Whence) {

    return static_cast<off_t>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Lseek,
        static_cast<uintptr_t>(FileDescriptor),
        static_cast<uintptr_t>(Offset),
        static_cast<uintptr_t>(Whence)
    ));

};
