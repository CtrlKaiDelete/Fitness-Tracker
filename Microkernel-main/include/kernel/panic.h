#pragma once

#include "Microkernel/include/libkern/exceptionframe.h"

#include <cstdint>

namespace Microkernel {

    enum class PanicReason : uint32_t {

        Unknown = 0,
        NonMaskableInterrupt = 1,
        HardFault = 2,
        SegmentationFault = 3,
        BusFault = 4,
        UsageFault = 5,
        DebugMonitorFault = 6,
        PendSVFault = 7,
        SysTickFault = 8,
        PageFault = 9,
        PagePermissionFault = 10,
        DriverTimeout = 11

    };

    struct PanicInfo {

        PanicReason Reason = PanicReason::Unknown;
        uint32_t ExceptionReturn = 0;
        uint32_t FaultStatus = 0;
        uint32_t HardFaultStatus = 0;
        uint32_t FaultAddress = 0;
        uint32_t LinkRegister = 0;
        uint32_t ProgramCounter = 0;

    };

};
