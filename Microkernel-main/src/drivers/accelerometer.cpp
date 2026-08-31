#include "Microkernel/include/drivers/accelerometer.h"
#include "Microkernel/include/drivers/internali2c.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    namespace {

        constexpr uint8_t IdentityRegister = 0x0F;
        constexpr uint8_t ControlRegisterOne = 0x20;
        constexpr uint8_t ControlRegisterFour = 0x23;
        constexpr uint8_t OutputXLow = 0x28;
        constexpr uint8_t AutoIncrement = 1u << 7;
        constexpr uint8_t FiftyHertzAllAxes = 0x47;
        constexpr uint8_t BlockUpdateHighResolution = 0x88;

    };

    void AccelerometerDriver::OnStart() {

        this->Available = false;
        this->Identity = 0;
        if(!this->ReadRegisters(IdentityRegister, &this->Identity, 1) ||
            this->Identity != this->ExpectedIdentity) {

            return;

        };

        this->Available =
            this->WriteRegister(ControlRegisterOne, FiftyHertzAllAxes) &&
            this->WriteRegister(ControlRegisterFour, BlockUpdateHighResolution);

    };

    bool AccelerometerDriver::WriteRegister(uint8_t Register, uint8_t Value) {

        uint8_t Data[] = {Register, Value};
        auto& Bus = this->Kernel->DeviceManager.LoadDriver<InternalI2CDriver>();
        return Bus.Transmit(this->Address, Data, sizeof(Data));

    };

    bool AccelerometerDriver::ReadRegisters(uint8_t Register, uint8_t* Data, size_t Size) {

        auto& Bus = this->Kernel->DeviceManager.LoadDriver<InternalI2CDriver>();
        uint8_t Address = Size > 1 ? static_cast<uint8_t>(Register | AutoIncrement) : Register;
        return Bus.WriteRead(this->Address, &Address, 1, Data, Size);

    };

    bool AccelerometerDriver::ReadAcceleration(AccelerationSample& Sample) {

        DriverWatchdogScope Watchdog(*this);
        uint8_t Data[6] = {};
        if(!this->Available || !this->ReadRegisters(OutputXLow, Data, sizeof(Data))) {

            return false;

        };

        Sample.X = static_cast<int16_t>(static_cast<uint16_t>(Data[0]) |
            static_cast<uint16_t>(Data[1]) << 8) >> 4;
        Sample.Y = static_cast<int16_t>(static_cast<uint16_t>(Data[2]) |
            static_cast<uint16_t>(Data[3]) << 8) >> 4;
        Sample.Z = static_cast<int16_t>(static_cast<uint16_t>(Data[4]) |
            static_cast<uint16_t>(Data[5]) << 8) >> 4;
        return true;

    };

    bool AccelerometerDriver::IsAvailable() const {

        return this->Available;

    };

    uint8_t AccelerometerDriver::GetIdentity() const {

        return this->Identity;

    };

};
