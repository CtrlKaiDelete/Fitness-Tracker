// Created by Julien Rodot on 28/8/26. Licensed under MIT

#pragma once

#include "Microkernel/include/kernel/timer.h"
#include "Microkernel/include/drivers/driver.h"
#include "Microkernel/include/kernel/memory.h"
#include "Microkernel/include/kernel/panic.h"
#include "Microkernel/include/mefi/mefi.h"
#include "Microkernel/include/kernel/scheduler.h"
#include "Microkernel/include/kernel/devicemanager.h"
#include "Microkernel/include/kernel/syscallmanager.h"
#include "Microkernel/include/kernel/interruptmanager.h"
#include "Microkernel/include/kernel/watchdog.h"
#include "Microkernel/include/kernel/process.h"
#include "Microkernel/include/kernel/filesystem.h"

namespace Microkernel {

    class Kernel {

    public:

        static Kernel* New();

        Kernel() = default;
        Kernel(const Kernel&) = delete;
        virtual ~Kernel() = default;

        bool AttachFirmware(const MEFI::SystemTable* Firmware);
        [[nodiscard]] const MEFI::SystemTable* GetFirmware() const;

        [[noreturn]] void Panic(const char* Error);
        [[noreturn]] void Panic(PanicReason Reason);
        [[noreturn]] void Panic(PanicReason Reason, const ExceptionFrame* Frame, uint32_t ExceptionReturn);

        Microkernel::Timer InterruptTimer;
        Microkernel::MemoryManager MemoryManager;
        Microkernel::Scheduler Scheduler;
        Microkernel::DeviceManager DeviceManager;
        Microkernel::DriverWatchdog DriverWatchdog;
        Microkernel::SyscallManager SyscallManager;
        Microkernel::InterruptManager InterruptManager;
        Microkernel::ProcessTable Processes;
        Microkernel::FileSystem Filesystem;
        Microkernel::IPCManager IPC;
        volatile PanicInfo LastPanic = {};

    private:

        void StopDriversForPanic();

        const MEFI::SystemTable* FirmwareInterface = nullptr;

    };

};
