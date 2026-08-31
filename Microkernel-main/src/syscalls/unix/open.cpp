#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <cstdarg>
#include <fcntl.h>

namespace Microkernel {

    uintptr_t OpenSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        char Path[64] = {};
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr) {

            return this->Fail(EPERM);

        };

        int Error = 0;
        int Flags = static_cast<int>(Arg1);
        OpenFile* File = this->Kernel->Filesystem.Open(Path, Flags, static_cast<uint16_t>(Arg2), Error);
        if(File == nullptr) {

            return this->Fail(Error);

        };

        Capability Rights = Capability::Transfer;
        if((Flags & O_ACCMODE) != O_WRONLY) {

            Rights = Rights | Capability::Read;

        };

        if((Flags & O_ACCMODE) != O_RDONLY) {

            Rights = Rights | Capability::Write;

        };

        uint32_t Descriptor = Current->GetHandles().Add(*File, Rights);
        return Descriptor == 0 ? this->Fail(EMFILE) : Descriptor;

    };

};

extern "C" int _open(const char* Path, int Flags, ...) {

    uintptr_t Mode = 0;
    if((Flags & O_CREAT) != 0) {

        va_list Arguments;
        va_start(Arguments, Flags);
        Mode = static_cast<uintptr_t>(va_arg(Arguments, int));
        va_end(Arguments);

    };

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Open,
        reinterpret_cast<uintptr_t>(Path),
        static_cast<uintptr_t>(Flags),
        Mode
    ));

};
