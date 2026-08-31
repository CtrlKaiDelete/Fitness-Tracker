#pragma once

#include "Microkernel/include/syscalls/syscall.h"

#include <cstdint>

namespace Microkernel {

    class WritePixel : public Syscall {

    public:

        WritePixel() {

            this->SyscallNumber = 7;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

};
