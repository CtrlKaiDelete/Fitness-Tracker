#include "Microkernel/include/kernel/watchdog.h"
#include "Microkernel/include/drivers/driver.h"
#include "Microkernel/include/kernel/kernel.h"
#include "Microkernel/include/kernel/panic.h"

namespace Microkernel {

    namespace {

        constexpr uintptr_t Timer2 = 0x4000A000;
        constexpr uintptr_t TaskStart = Timer2 + 0x000;
        constexpr uintptr_t TaskStop = Timer2 + 0x004;
        constexpr uintptr_t TaskClear = Timer2 + 0x00C;
        constexpr uintptr_t CompareEvent = Timer2 + 0x140;
        constexpr uintptr_t Shorts = Timer2 + 0x200;
        constexpr uintptr_t InterruptClear = Timer2 + 0x308;
        constexpr uintptr_t InterruptSet = Timer2 + 0x304;
        constexpr uintptr_t Mode = Timer2 + 0x504;
        constexpr uintptr_t BitMode = Timer2 + 0x508;
        constexpr uintptr_t Prescaler = Timer2 + 0x510;
        constexpr uintptr_t Compare = Timer2 + 0x540;

        constexpr uintptr_t NvicEnable = 0xE000E100;
        constexpr uintptr_t NvicDisable = 0xE000E180;
        constexpr uintptr_t NvicPendingClear = 0xE000E280;
        constexpr uintptr_t NvicPriority = 0xE000E400;

        constexpr uint32_t InterruptNumber = 10;
        constexpr uint32_t InterruptMask = 1u << InterruptNumber;
        constexpr uint32_t CompareInterruptMask = 1u << 16;
        constexpr uint32_t CompareStopShortcut = 1u << 8;

        uint32_t DisableInterrupts() {

            uint32_t State = 0;
            __asm__ volatile("mrs %0, primask" : "=r"(State));
            __asm__ volatile("cpsid i" ::: "memory");
            return State;

        };

        void RestoreInterrupts(uint32_t State) {

            if(State == 0) {

                __asm__ volatile("cpsie i" ::: "memory");

            };

        };

    };

    bool DriverWatchdog::Initialize(uint32_t TimeoutMicroseconds) {

        if(TimeoutMicroseconds == 0) {

            return false;

        };

        uint32_t InterruptState = DisableInterrupts();

        *reinterpret_cast<volatile uint32_t*>(NvicDisable) = InterruptMask;
        *reinterpret_cast<volatile uint32_t*>(TaskStop) = 1;
        *reinterpret_cast<volatile uint32_t*>(InterruptClear) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(CompareEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(Mode) = 0;
        *reinterpret_cast<volatile uint32_t*>(BitMode) = 3;
        *reinterpret_cast<volatile uint32_t*>(Prescaler) = 4;
        *reinterpret_cast<volatile uint32_t*>(Shorts) = CompareStopShortcut;
        *reinterpret_cast<volatile uint32_t*>(Compare) = TimeoutMicroseconds;
        *reinterpret_cast<volatile uint32_t*>(InterruptSet) = CompareInterruptMask;
        *reinterpret_cast<volatile uint8_t*>(NvicPriority + InterruptNumber) = 0x00;
        *reinterpret_cast<volatile uint32_t*>(NvicPendingClear) = InterruptMask;
        *reinterpret_cast<volatile uint32_t*>(NvicEnable) = InterruptMask;

        this->Timeout = TimeoutMicroseconds;
        this->Depth = 0;
        this->Initialized = true;

        RestoreInterrupts(InterruptState);
        return true;

    };

    void DriverWatchdog::RestartTimer() {

        *reinterpret_cast<volatile uint32_t*>(TaskStop) = 1;
        *reinterpret_cast<volatile uint32_t*>(CompareEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(NvicPendingClear) = InterruptMask;
        *reinterpret_cast<volatile uint32_t*>(Compare) = this->Timeout;
        *reinterpret_cast<volatile uint32_t*>(TaskClear) = 1;
        *reinterpret_cast<volatile uint32_t*>(TaskStart) = 1;

    };

    void DriverWatchdog::Arm(Driver& Driver) {

        if(!this->Initialized) {

            return;

        };

        uint32_t InterruptState = DisableInterrupts();
        bool StartTimer = this->Depth == 0;

        if(this->Depth < this->MaximumDepth) {

            std::size_t Index = this->Depth;
            this->DriverStack[Index] = &Driver;
            this->Depth = Index + 1;

        } else {

            this->DriverStack[this->MaximumDepth - 1] = &Driver;

        };

        if(StartTimer) {

            this->RestartTimer();

        };

        RestoreInterrupts(InterruptState);

    };

    void DriverWatchdog::Disarm(Driver& Driver) {

        if(!this->Initialized) {

            return;

        };

        uint32_t InterruptState = DisableInterrupts();
        bool TimedOut = *reinterpret_cast<volatile uint32_t*>(CompareEvent) != 0;
        bool StackValid = true;

        if(this->Depth > 0 && this->DriverStack[this->Depth - 1] == &Driver) {

            std::size_t NewDepth = this->Depth - 1;
            this->Depth = NewDepth;
            this->DriverStack[NewDepth] = nullptr;

        } else {

            StackValid = false;

            for(std::size_t Index = 0; Index < this->MaximumDepth; Index++) {

                this->DriverStack[Index] = nullptr;

            };

            this->Depth = 0;

        };

        if(!StackValid || this->Depth == 0) {

            *reinterpret_cast<volatile uint32_t*>(TaskStop) = 1;
            *reinterpret_cast<volatile uint32_t*>(CompareEvent) = 0;
            *reinterpret_cast<volatile uint32_t*>(NvicPendingClear) = InterruptMask;

        };

        RestoreInterrupts(InterruptState);

        if(TimedOut) {

            this->Kernel->Panic(PanicReason::DriverTimeout);

        };

    };

    void DriverWatchdog::Stop() {

        uint32_t InterruptState = DisableInterrupts();

        *reinterpret_cast<volatile uint32_t*>(TaskStop) = 1;
        *reinterpret_cast<volatile uint32_t*>(InterruptClear) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(CompareEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(NvicDisable) = InterruptMask;
        *reinterpret_cast<volatile uint32_t*>(NvicPendingClear) = InterruptMask;

        for(std::size_t Index = 0; Index < this->MaximumDepth; Index++) {

            this->DriverStack[Index] = nullptr;

        };

        this->Depth = 0;
        this->Initialized = false;

        RestoreInterrupts(InterruptState);

    };

    void DriverWatchdog::HandleInterrupt() {

        volatile uint32_t* Event = reinterpret_cast<volatile uint32_t*>(CompareEvent);
        if(*Event == 0) {

            return;

        };

        *Event = 0;
        if(this->Depth == 0) {

            return;

        };

        this->Kernel->Panic(PanicReason::DriverTimeout);

    };

    bool DriverWatchdog::IsArmed() const {

        return this->Initialized && this->Depth > 0;

    };

    const Driver* DriverWatchdog::GetActiveDriver() const {

        return this->Depth == 0 ? nullptr : this->DriverStack[this->Depth - 1];

    };

};

extern "C" void kernel_watchdog_dispatch() {

    Microkernel::Kernel::New()->DriverWatchdog.HandleInterrupt();

};
