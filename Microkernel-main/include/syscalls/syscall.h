#pragma once

#include "Microkernel/include/base/object.h"
#include "Microkernel/include/kernel/memory.h"

#include <cstdint>
#include <cstddef>

namespace Microkernel {

    class Syscall : public Object {

    public:

        virtual ~Syscall() = default;
        virtual uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) = 0;
        uint32_t SyscallNumber = 0;

    protected:

        bool ValidateRead(const void* Pointer, size_t Size) const;
        bool ValidateWrite(void* Pointer, size_t Size) const;
        bool ValidateString(const char* String, size_t MaximumLength) const;

    };

};
