#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <cstddef>
#include <cstdint>

namespace Microkernel {

    uintptr_t SbrkSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        ptrdiff_t Increment = static_cast<ptrdiff_t>(Arg0);
        void* PreviousHeapEnd = this->Kernel->MemoryManager.AdjustHeap(Increment);
        if(PreviousHeapEnd == nullptr) {

            return this->Fail(ENOMEM);

        };

        return reinterpret_cast<uintptr_t>(PreviousHeapEnd);

    };

};

extern "C" void* _sbrk(ptrdiff_t Increment) {

    void* Result = Microkernel::Kernel::New()->MemoryManager.AdjustHeap(Increment);
    if(Result == nullptr) {

        errno = ENOMEM;
        return reinterpret_cast<void*>(-1);

    };

    return Result;

};
