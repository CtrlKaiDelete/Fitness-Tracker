#include "Microkernel/include/syscalls/syscall.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    bool Syscall::ValidateRead(const void* Pointer, size_t Size) const {

        Process* Current = this->Kernel->Processes.GetCurrent();
        return Current == nullptr ? Pointer != nullptr || Size == 0 : Current->ValidatePointer(Pointer, Size, PagePermission::Read);

    };

    bool Syscall::ValidateWrite(void* Pointer, size_t Size) const {

        Process* Current = this->Kernel->Processes.GetCurrent();
        return Current == nullptr ? Pointer != nullptr || Size == 0 : Current->ValidatePointer(Pointer, Size, PagePermission::Write);

    };

    bool Syscall::ValidateString(const char* String, size_t MaximumLength) const {

        if(String == nullptr || MaximumLength == 0) {

            return false;

        };

        for(size_t Index = 0; Index < MaximumLength; Index++) {

            if(!this->ValidateRead(&String[Index], sizeof(char))) {

                return false;

            };

            if(String[Index] == '\0') {

                return true;

            };

        };

        return false;

    };

};
