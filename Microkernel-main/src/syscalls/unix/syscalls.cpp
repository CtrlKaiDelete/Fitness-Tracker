#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t UnixSyscall::Fail(int Error) {

        errno = Error;
        return static_cast<uintptr_t>(-1);

    };

    bool UnixSyscall::IsStandardDescriptor(int FileDescriptor) {

        return FileDescriptor >= 0 && FileDescriptor <= 2;

    };

    bool UnixSyscall::CopyUserString(const char* Source, char* Destination, size_t Capacity) const {

        if(Source == nullptr || Destination == nullptr || Capacity == 0) {

            return false;

        };

        for(size_t Index = 0; Index < Capacity; Index++) {

            if(!this->ValidateRead(&Source[Index], 1)) {

                return false;

            };

            Destination[Index] = Source[Index];
            if(Destination[Index] == '\0') {

                return true;

            };

        };

        Destination[Capacity - 1] = '\0';
        return false;

    };

    uintptr_t InvokeUnixSyscall(uint32_t Number, uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        register uintptr_t R0 asm("r0") = Number;
        register uintptr_t R1 asm("r1") = Arg0;
        register uintptr_t R2 asm("r2") = Arg1;
        register uintptr_t R3 asm("r3") = Arg2;
        __asm__ volatile("svc #0" : "+r"(R0) : "r"(R1), "r"(R2), "r"(R3) : "memory");
        return R0;

    };

};
