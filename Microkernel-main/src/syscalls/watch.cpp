#include "Microkernel/include/syscalls/watch.h"
#include "Microkernel/include/drivers/accelerometer.h"
#include "Microkernel/include/drivers/bluetooth.h"
#include "Microkernel/include/drivers/button.h"
#include "Microkernel/include/drivers/logo.h"
#include "Microkernel/include/drivers/magnetometer.h"
#include "Microkernel/include/drivers/sound.h"
#include "Microkernel/include/kernel/kernel.h"

#include <bit>

namespace Microkernel {

    ReadControlsSyscall::ReadControlsSyscall() {

        this->SyscallNumber = WatchSyscallNumber::ReadControls;

    };

    uintptr_t ReadControlsSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        auto& Buttons = this->Kernel->DeviceManager.LoadDriver<ButtonDriver>();
        auto& Logo = this->Kernel->DeviceManager.LoadDriver<LogoDriver>();
        uintptr_t State = 0;
        if(Buttons.IsPressed(Button::A)) {

            State |= 1u << 0;

        };

        if(Buttons.IsPressed(Button::B)) {

            State |= 1u << 1;

        };

        if(Logo.IsPressed()) {

            State |= 1u << 2;

        };

        return State;

    };

    PlayToneSyscall::PlayToneSyscall() {

        this->SyscallNumber = WatchSyscallNumber::PlayTone;

    };

    uintptr_t PlayToneSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr || Current->GetIdentifier() != 3) {

            return static_cast<uintptr_t>(-1);

        };

        float Volume = std::bit_cast<float>(static_cast<uint32_t>(Arg1));
        auto& Sound = this->Kernel->DeviceManager.LoadDriver<SoundDriver>();
        return Sound.PlayTone(static_cast<uint32_t>(Arg0), Volume) ? 0 : static_cast<uintptr_t>(-1);

    };

    StopToneSyscall::StopToneSyscall() {

        this->SyscallNumber = WatchSyscallNumber::StopTone;

    };

    uintptr_t StopToneSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr || Current->GetIdentifier() != 3) {

            return static_cast<uintptr_t>(-1);

        };

        auto& Sound = this->Kernel->DeviceManager.LoadDriver<SoundDriver>();
        Sound.Stop();
        return 0;

    };

    UpdateBluetoothSyscall::UpdateBluetoothSyscall() {

        this->SyscallNumber = WatchSyscallNumber::UpdateBluetooth;

    };

    uintptr_t UpdateBluetoothSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr || Current->GetIdentifier() != 6) {

            return static_cast<uintptr_t>(-1);

        };

        auto& Bluetooth = this->Kernel->DeviceManager.LoadDriver<BluetoothDriver>();
        Bluetooth.SetKeyboardReport(static_cast<uint8_t>(Arg0), static_cast<uint8_t>(Arg1));
        Bluetooth.Update();

        uintptr_t State = 0;
        if(Bluetooth.IsAdvertising()) {

            State |= 1u << 0;

        };

        if(Bluetooth.IsConnected()) {

            State |= 1u << 1;

        };

        return State;

    };

    GetTicksSyscall::GetTicksSyscall() {

        this->SyscallNumber = WatchSyscallNumber::GetTicks;

    };

    uintptr_t GetTicksSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        return static_cast<uintptr_t>(this->Kernel->InterruptTimer.GetTicks());

    };

    DebugPanicSyscall::DebugPanicSyscall() {

        this->SyscallNumber = WatchSyscallNumber::DebugPanic;

    };

    uintptr_t DebugPanicSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr || Current->GetIdentifier() != 1) {

            return static_cast<uintptr_t>(-1);

        };

        this->Kernel->Panic("INIT PANIC");

    };

    YieldSyscall::YieldSyscall() {

        this->SyscallNumber = WatchSyscallNumber::Yield;

    };

    uintptr_t YieldSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        this->Kernel->Scheduler.Yield();
        return 0;

    };

    ReadAccelerationSyscall::ReadAccelerationSyscall() {

        this->SyscallNumber = WatchSyscallNumber::ReadAcceleration;

    };

    uintptr_t ReadAccelerationSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto Sample = reinterpret_cast<AccelerationSample*>(Arg0);
        if(Current == nullptr || Current->GetIdentifier() != 2 ||
            !this->ValidateWrite(Sample, sizeof(AccelerationSample))) {

            return static_cast<uintptr_t>(-1);

        };

        AccelerationSample Result = {};
        auto& Accelerometer = this->Kernel->DeviceManager.LoadDriver<AccelerometerDriver>();
        if(!Accelerometer.ReadAcceleration(Result)) {

            return static_cast<uintptr_t>(-1);

        };

        *Sample = Result;
        return 0;

    };

    ReadMagneticFieldSyscall::ReadMagneticFieldSyscall() {

        this->SyscallNumber = WatchSyscallNumber::ReadMagneticField;

    };

    uintptr_t ReadMagneticFieldSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto Sample = reinterpret_cast<MagneticFieldSample*>(Arg0);
        if(Current == nullptr || Current->GetIdentifier() != 2 ||
            !this->ValidateWrite(Sample, sizeof(MagneticFieldSample))) {

            return static_cast<uintptr_t>(-1);

        };

        MagneticFieldSample Result = {};
        auto& Magnetometer = this->Kernel->DeviceManager.LoadDriver<MagnetometerDriver>();
        if(!Magnetometer.ReadMagneticField(Result)) {

            return static_cast<uintptr_t>(-1);

        };

        *Sample = Result;
        return 0;

    };

};
