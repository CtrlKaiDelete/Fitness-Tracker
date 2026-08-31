#pragma once

#include <cstdint>

namespace Microkernel {

    struct ExceptionFrame {

        uint32_t R0;
        uint32_t R1;
        uint32_t R2;
        uint32_t R3;
        uint32_t R12;
        uint32_t LC;
        uint32_t PC;
        uint32_t XPSR;

    };

};