#include "Microkernel/include/kernel/panic.h"
#include "Microkernel/include/kernel/kernel.h"
#include "Microkernel/include/libkern/font.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    class PanicDisplay {

    public:

        PanicDisplay() {

            *reinterpret_cast<volatile uint32_t*>(Timer0Stop) = 1;
            *reinterpret_cast<volatile uint32_t*>(Timer1Stop) = 1;
            *reinterpret_cast<volatile uint32_t*>(Timer2Stop) = 1;
            *reinterpret_cast<volatile uint32_t*>(Pwm0Stop) = 1;
            *reinterpret_cast<volatile uint32_t*>(Pwm1Stop) = 1;
            *reinterpret_cast<volatile uint32_t*>(Pwm2Stop) = 1;
            *reinterpret_cast<volatile uint32_t*>(SysTickControl) = 0;

            *reinterpret_cast<volatile uint32_t*>(Port0DirectionSet) = MatrixPort0Mask;
            *reinterpret_cast<volatile uint32_t*>(Port1DirectionSet) = ColumnPort1Mask;

            *reinterpret_cast<volatile uint32_t*>(SysTickReload) = 63999;
            *reinterpret_cast<volatile uint32_t*>(SysTickCurrent) = 0;
            *reinterpret_cast<volatile uint32_t*>(SysTickControl) = 5;

            this->Clear();

        };

        void Clear() {

            for(int Row = 0; Row < Font::Height; Row++) {

                this->Framebuffer[Row] = 0;

            };

        };

        void ShowSadFace() {

            static constexpr uint8_t SadFace[Font::Height] = {

                0b10101,
                0b01010,
                0b10101,
                0b01110,
                0b10001

            };

            for(int Row = 0; Row < Font::Height; Row++) {

                this->Framebuffer[Row] = SadFace[Row];

            };

        };

        void Hold(uint32_t Milliseconds) {

            int Row = 0;
            for(uint32_t Millisecond = 0; Millisecond < Milliseconds; Millisecond++) {

                this->Scan(Row);
                this->WaitForTick();

                Row++;
                if(Row >= Font::Height) {

                    Row = 0;

                };

            };

        };

        void Scroll(const char* Text) {

            int Length = 0;
            while(Text[Length] != '\0') {

                Length++;

            };

            for(int Offset = -Font::Width; Offset < (Length * (Font::Width + 1)); Offset++) {

                this->DrawTextFrame(Text, Length, Offset);
                this->Hold(100);

            };

        };

    private:

        void DrawTextFrame(const char* Text, int Length, int Offset) {

            this->Clear();

            for(int X = 0; X < Font::Width; X++) {

                int TextColumn = Offset + X;
                if(TextColumn < 0) {

                    continue;

                };

                int Character = TextColumn / (Font::Width + 1);
                int GlyphColumn = TextColumn % (Font::Width + 1);
                if(Character >= Length || GlyphColumn >= Font::Width) {

                    continue;

                };

                for(int Row = 0; Row < Font::Height; Row++) {

                    uint8_t GlyphRow = this->PanicFont.GetRow(Text[Character], Row);
                    if((GlyphRow & (1u << ((Font::Width - 1) - GlyphColumn))) != 0) {

                        this->Framebuffer[Row] |= 1u << X;

                    };

                };

            };

        };

        void Scan(int Row) {

            *reinterpret_cast<volatile uint32_t*>(Port0OutputClear) = RowMask;
            *reinterpret_cast<volatile uint32_t*>(Port0OutputSet) = ColumnPort0Mask;
            *reinterpret_cast<volatile uint32_t*>(Port1OutputSet) = ColumnPort1Mask;

            uint8_t Pixels = this->Framebuffer[Row];
            for(int Column = 0; Column < Font::Width; Column++) {

                if((Pixels & (1u << Column)) == 0) {

                    continue;

                };

                if(ColumnPorts[Column] == 0) {

                    *reinterpret_cast<volatile uint32_t*>(Port0OutputClear) = 1u << ColumnPins[Column];

                } else {

                    *reinterpret_cast<volatile uint32_t*>(Port1OutputClear) = 1u << ColumnPins[Column];

                };

            };

            *reinterpret_cast<volatile uint32_t*>(Port0OutputSet) = 1u << RowPins[Row];

        };

        void WaitForTick() {

            volatile uint32_t* Control = reinterpret_cast<volatile uint32_t*>(SysTickControl);
            while((*Control & (1u << 16)) == 0) {};

        };

        static constexpr uintptr_t Port0OutputSet = 0x50000508;
        static constexpr uintptr_t Port0OutputClear = 0x5000050C;
        static constexpr uintptr_t Port0DirectionSet = 0x50000518;
        static constexpr uintptr_t Port1OutputSet = 0x50000808;
        static constexpr uintptr_t Port1OutputClear = 0x5000080C;
        static constexpr uintptr_t Port1DirectionSet = 0x50000818;
        static constexpr uintptr_t Timer0Stop = 0x40008004;
        static constexpr uintptr_t Timer1Stop = 0x40009004;
        static constexpr uintptr_t Timer2Stop = 0x4000A004;
        static constexpr uintptr_t Pwm0Stop = 0x4001C004;
        static constexpr uintptr_t Pwm1Stop = 0x40021004;
        static constexpr uintptr_t Pwm2Stop = 0x40022004;
        static constexpr uintptr_t SysTickControl = 0xE000E010;
        static constexpr uintptr_t SysTickReload = 0xE000E014;
        static constexpr uintptr_t SysTickCurrent = 0xE000E018;

        static constexpr uint32_t RowMask = 0x01688000;
        static constexpr uint32_t ColumnPort0Mask = 0xD0000800;
        static constexpr uint32_t ColumnPort1Mask = 0x00000020;
        static constexpr uint32_t MatrixPort0Mask = 0xD1688800;

        static constexpr int RowPins[Font::Height] = {21, 22, 15, 24, 19};
        static constexpr int ColumnPorts[Font::Width] = {0, 0, 0, 1, 0};
        static constexpr int ColumnPins[Font::Width] = {28, 11, 31, 5, 30};

        Font PanicFont;
        uint8_t Framebuffer[Font::Height] = {};

    };

    static const char* GetPanicMessage(PanicReason Reason, uint32_t FaultStatus) {

        if(Reason == PanicReason::HardFault) {

            if((FaultStatus & 0x000000FF) != 0) {

                Reason = PanicReason::SegmentationFault;

            } else if((FaultStatus & 0x0000FF00) != 0) {

                Reason = PanicReason::BusFault;

            } else if((FaultStatus & 0xFFFF0000) != 0) {

                Reason = PanicReason::UsageFault;

            };

        };

        if(Reason == PanicReason::UsageFault) {

            if((FaultStatus & (1u << 25)) != 0) return "DIVIDE BY ZERO";
            if((FaultStatus & (1u << 24)) != 0) return "UNALIGNED ACCESS";
            if((FaultStatus & (1u << 19)) != 0) return "NO COPROCESSOR";
            if((FaultStatus & (1u << 18)) != 0) return "INVALID RETURN";
            if((FaultStatus & (1u << 17)) != 0) return "INVALID STATE";
            if((FaultStatus & (1u << 16)) != 0) return "UNDEFINED INSTRUCTION";

        };

        switch(Reason) {

            case PanicReason::NonMaskableInterrupt: return "NON MASKABLE INTERRUPT";
            case PanicReason::HardFault: return "HARD FAULT";
            case PanicReason::SegmentationFault: return "SEGMENTATION FAULT";
            case PanicReason::BusFault: return "BUS FAULT";
            case PanicReason::UsageFault: return "USAGE FAULT";
            case PanicReason::DebugMonitorFault: return "DEBUG MONITOR FAULT";
            case PanicReason::PendSVFault: return "UNHANDLED PENDSV";
            case PanicReason::SysTickFault: return "UNHANDLED SYSTICK";
            case PanicReason::PageFault: return "PAGE FAULT";
            case PanicReason::PagePermissionFault: return "PAGE PERMISSION FAULT";
            case PanicReason::DriverTimeout: return "DRIVER TIMEOUT";
            default: return "KERNEL PANIC";

        };

    };

    [[noreturn]] static void ShowPanic(const char* Error) {

        PanicDisplay Display;

        while(true) {

            Display.ShowSadFace();
            Display.Hold(1000);
            Display.Clear();
            Display.Hold(250);
            Display.Scroll(Error);
            Display.Clear();
            Display.Hold(750);

        };

    };

    void Kernel::StopDriversForPanic() {

        this->Scheduler.Stop();
        this->DriverWatchdog.Stop();
        this->InterruptManager.DisableAll();
        this->InterruptTimer.Stop();
        this->DeviceManager.ForceStopAll();

    };

    [[noreturn]] void Kernel::Panic(const char* Error) {

        __asm__ volatile("cpsid i" ::: "memory");

        this->LastPanic.Reason = PanicReason::Unknown;
        this->StopDriversForPanic();
        ShowPanic(Error == nullptr ? "KERNEL PANIC" : Error);

    };

    [[noreturn]] void Kernel::Panic(PanicReason Reason) {

        __asm__ volatile("cpsid i" ::: "memory");

        this->LastPanic.Reason = Reason;
        this->StopDriversForPanic();
        ShowPanic(GetPanicMessage(Reason, 0));

    };

    [[noreturn]] void Kernel::Panic(PanicReason Reason, const ExceptionFrame* Frame, uint32_t ExceptionReturn) {

        __asm__ volatile("cpsid i" ::: "memory");

        uint32_t FaultStatus = *reinterpret_cast<volatile uint32_t*>(0xE000ED28);

        this->LastPanic.Reason = Reason;
        this->LastPanic.ExceptionReturn = ExceptionReturn;
        this->LastPanic.FaultStatus = FaultStatus;
        this->LastPanic.HardFaultStatus = *reinterpret_cast<volatile uint32_t*>(0xE000ED2C);

        if((FaultStatus & (1u << 7)) != 0) {

            this->LastPanic.FaultAddress = *reinterpret_cast<volatile uint32_t*>(0xE000ED34);

        } else if((FaultStatus & (1u << 15)) != 0) {

            this->LastPanic.FaultAddress = *reinterpret_cast<volatile uint32_t*>(0xE000ED38);

        };

        uintptr_t FrameAddress = reinterpret_cast<uintptr_t>(Frame);
        bool StackFrameValid = (FaultStatus & ((1u << 4) | (1u << 5) | (1u << 12) | (1u << 13))) == 0;
        if(StackFrameValid && FrameAddress >= 0x20000000 && FrameAddress <= (0x20020000 - sizeof(ExceptionFrame))) {

            this->LastPanic.LinkRegister = Frame->LC;
            this->LastPanic.ProgramCounter = Frame->PC;

        };

        this->StopDriversForPanic();
        ShowPanic(GetPanicMessage(Reason, FaultStatus));

    };

};

extern "C" [[noreturn]] void kernel_fault_dispatch(uint32_t Reason, Microkernel::ExceptionFrame* Frame, uint32_t ExceptionReturn) {

    Microkernel::Kernel::New()->Panic(static_cast<Microkernel::PanicReason>(Reason), Frame, ExceptionReturn);

};

extern "C" void kernel_memory_fault_dispatch(Microkernel::ExceptionFrame* Frame, uint32_t ExceptionReturn) {

    Microkernel::Kernel* Kernel = Microkernel::Kernel::New();
    Microkernel::PageFaultResult Result = Kernel->MemoryManager.HandlePageFault(Frame);
    if(Result == Microkernel::PageFaultResult::Handled) {

        return;

    };

    Microkernel::PanicReason Reason = Result == Microkernel::PageFaultResult::PermissionDenied
        ? Microkernel::PanicReason::PagePermissionFault
        : Microkernel::PanicReason::PageFault;

    Kernel->Panic(Reason, Frame, ExceptionReturn);

};
