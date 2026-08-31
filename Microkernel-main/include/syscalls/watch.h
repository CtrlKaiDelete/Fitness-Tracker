#pragma once

#include "Microkernel/include/syscalls/syscall.h"

#include <cstdint>

namespace Microkernel {

    namespace WatchSyscallNumber {

        constexpr uint32_t ReadControls = 8;
        constexpr uint32_t PlayTone = 9;
        constexpr uint32_t StopTone = 10;
        constexpr uint32_t UpdateBluetooth = 11;
        constexpr uint32_t GetTicks = 12;
        constexpr uint32_t DebugPanic = 13;
        constexpr uint32_t Yield = 14;
        constexpr uint32_t ReadAcceleration = 15;
        constexpr uint32_t ReadMagneticField = 18;

    };

    class ReadControlsSyscall : public Syscall {

    public:

        ReadControlsSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class PlayToneSyscall : public Syscall {

    public:

        PlayToneSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class StopToneSyscall : public Syscall {

    public:

        StopToneSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class UpdateBluetoothSyscall : public Syscall {

    public:

        UpdateBluetoothSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class GetTicksSyscall : public Syscall {

    public:

        GetTicksSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class DebugPanicSyscall : public Syscall {

    public:

        DebugPanicSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class YieldSyscall : public Syscall {

    public:

        YieldSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class ReadAccelerationSyscall : public Syscall {

    public:

        ReadAccelerationSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class ReadMagneticFieldSyscall : public Syscall {

    public:

        ReadMagneticFieldSyscall();
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

};
