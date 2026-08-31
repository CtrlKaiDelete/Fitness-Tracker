#include "Microkernel/include/drivers/internali2c.h"

namespace Microkernel {

    namespace {

        constexpr uintptr_t StartReceive = InternalI2CDriver::Peripheral + 0x000;
        constexpr uintptr_t StartTransmit = InternalI2CDriver::Peripheral + 0x008;
        constexpr uintptr_t StopTransaction = InternalI2CDriver::Peripheral + 0x014;
        constexpr uintptr_t StoppedEvent = InternalI2CDriver::Peripheral + 0x104;
        constexpr uintptr_t ErrorEvent = InternalI2CDriver::Peripheral + 0x124;
        constexpr uintptr_t Shortcuts = InternalI2CDriver::Peripheral + 0x200;
        constexpr uintptr_t ErrorSource = InternalI2CDriver::Peripheral + 0x4C4;
        constexpr uintptr_t Enable = InternalI2CDriver::Peripheral + 0x500;
        constexpr uintptr_t ClockPinSelect = InternalI2CDriver::Peripheral + 0x508;
        constexpr uintptr_t DataPinSelect = InternalI2CDriver::Peripheral + 0x50C;
        constexpr uintptr_t Frequency = InternalI2CDriver::Peripheral + 0x524;
        constexpr uintptr_t ReceivePointer = InternalI2CDriver::Peripheral + 0x534;
        constexpr uintptr_t ReceiveMaximum = InternalI2CDriver::Peripheral + 0x538;
        constexpr uintptr_t TransmitPointer = InternalI2CDriver::Peripheral + 0x544;
        constexpr uintptr_t TransmitMaximum = InternalI2CDriver::Peripheral + 0x548;
        constexpr uintptr_t AddressRegister = InternalI2CDriver::Peripheral + 0x588;

        constexpr uint32_t LastTransmitStartReceive = 1u << 7;
        constexpr uint32_t LastTransmitStop = 1u << 9;
        constexpr uint32_t LastReceiveStop = 1u << 12;
        constexpr uint32_t Enabled = 6;
        constexpr uint32_t Frequency400KHz = 0x06400000;
        constexpr uint32_t DisconnectedPin = 0xFFFFFFFF;
    };

    void InternalI2CDriver::OnStart() {

        *reinterpret_cast<volatile uint32_t*>(Enable) = 0;
        *reinterpret_cast<volatile uint32_t*>(ClockPinSelect) = this->SCLPin;
        *reinterpret_cast<volatile uint32_t*>(DataPinSelect) = this->SDAPin;
        *reinterpret_cast<volatile uint32_t*>(Frequency) = Frequency400KHz;
        *reinterpret_cast<volatile uint32_t*>(Enable) = Enabled;

    };

    void InternalI2CDriver::OnStop() {

        this->Stop();

    };

    void InternalI2CDriver::OnPanic() {

        this->Stop();

    };

    void InternalI2CDriver::Stop() {

        *reinterpret_cast<volatile uint32_t*>(StopTransaction) = 1;
        *reinterpret_cast<volatile uint32_t*>(Enable) = 0;
        *reinterpret_cast<volatile uint32_t*>(ClockPinSelect) = DisconnectedPin;
        *reinterpret_cast<volatile uint32_t*>(DataPinSelect) = DisconnectedPin;

    };

    bool InternalI2CDriver::WaitForTransaction() {

        while(*reinterpret_cast<volatile uint32_t*>(StoppedEvent) == 0) {

            if(*reinterpret_cast<volatile uint32_t*>(ErrorEvent) != 0) {

                *reinterpret_cast<volatile uint32_t*>(StopTransaction) = 1;
                while(*reinterpret_cast<volatile uint32_t*>(StoppedEvent) == 0) {

                    __asm__ volatile("nop");

                };

                return false;

            };

        };

        return *reinterpret_cast<volatile uint32_t*>(ErrorEvent) == 0;

    };

    bool InternalI2CDriver::Transmit(uint8_t Address, const uint8_t* Data, size_t Size) {

        DriverWatchdogScope Watchdog(*this);
        if(Data == nullptr || Size == 0) {

            return false;

        };

        *reinterpret_cast<volatile uint32_t*>(StoppedEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(ErrorEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(ErrorSource) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(Shortcuts) = LastTransmitStop;
        *reinterpret_cast<volatile uintptr_t*>(TransmitPointer) = reinterpret_cast<uintptr_t>(Data);
        *reinterpret_cast<volatile uint32_t*>(TransmitMaximum) = static_cast<uint32_t>(Size);
        *reinterpret_cast<volatile uint32_t*>(AddressRegister) = Address;
        *reinterpret_cast<volatile uint32_t*>(StartTransmit) = 1;
        return this->WaitForTransaction();

    };

    bool InternalI2CDriver::WriteRead(
        uint8_t Address,
        const uint8_t* WriteData,
        size_t WriteSize,
        uint8_t* ReadData,
        size_t ReadSize
    ) {

        DriverWatchdogScope Watchdog(*this);
        if(WriteData == nullptr || WriteSize == 0 || ReadData == nullptr || ReadSize == 0) {

            return false;

        };

        *reinterpret_cast<volatile uint32_t*>(StoppedEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(ErrorEvent) = 0;
        *reinterpret_cast<volatile uint32_t*>(ErrorSource) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(Shortcuts) = LastTransmitStartReceive | LastReceiveStop;
        *reinterpret_cast<volatile uintptr_t*>(TransmitPointer) = reinterpret_cast<uintptr_t>(WriteData);
        *reinterpret_cast<volatile uint32_t*>(TransmitMaximum) = static_cast<uint32_t>(WriteSize);
        *reinterpret_cast<volatile uintptr_t*>(ReceivePointer) = reinterpret_cast<uintptr_t>(ReadData);
        *reinterpret_cast<volatile uint32_t*>(ReceiveMaximum) = static_cast<uint32_t>(ReadSize);
        *reinterpret_cast<volatile uint32_t*>(AddressRegister) = Address;
        *reinterpret_cast<volatile uint32_t*>(StartTransmit) = 1;
        return this->WaitForTransaction();

    };

};
