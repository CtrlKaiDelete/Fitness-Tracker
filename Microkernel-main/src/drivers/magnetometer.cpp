#include "Microkernel/include/drivers/magnetometer.h"
#include "Microkernel/include/drivers/internali2c.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    namespace {

        constexpr uint8_t IdentityRegister = 0x4F;
        constexpr uint8_t ConfigurationRegisterA = 0x60;
        constexpr uint8_t ConfigurationRegisterC = 0x62;
        constexpr uint8_t OutputXLow = 0x68;
        constexpr uint8_t TemperatureCompensatedTenHertz = 0x80;
        constexpr uint8_t BlockDataUpdate = 0x10;

    };

    void MagnetometerDriver::OnStart() {

        this->Available = false;
        this->Identity = 0;
        if(!this->ReadRegisters(IdentityRegister, &this->Identity, 1) ||
            this->Identity != this->ExpectedIdentity) {

            return;

        };

        this->Available =
            this->WriteRegister(ConfigurationRegisterA, TemperatureCompensatedTenHertz) &&
            this->WriteRegister(ConfigurationRegisterC, BlockDataUpdate);

    };

    bool MagnetometerDriver::WriteRegister(uint8_t Register, uint8_t Value) {

        uint8_t Data[] = {Register, Value};
        auto& Bus = this->Kernel->DeviceManager.LoadDriver<InternalI2CDriver>();
        return Bus.Transmit(this->Address, Data, sizeof(Data));

    };

    bool MagnetometerDriver::ReadRegisters(uint8_t Register, uint8_t* Data, size_t Size) {

        auto& Bus = this->Kernel->DeviceManager.LoadDriver<InternalI2CDriver>();
        return Bus.WriteRead(this->Address, &Register, 1, Data, Size);

    };

    bool MagnetometerDriver::ReadMagneticField(MagneticFieldSample& Sample) {

        DriverWatchdogScope Watchdog(*this);
        uint8_t Data[6] = {};
        if(!this->Available || !this->ReadRegisters(OutputXLow, Data, sizeof(Data))) {

            return false;

        };

        Sample.X = static_cast<int16_t>(static_cast<uint16_t>(Data[0]) |
            static_cast<uint16_t>(Data[1]) << 8);
        Sample.Y = static_cast<int16_t>(static_cast<uint16_t>(Data[2]) |
            static_cast<uint16_t>(Data[3]) << 8);
        Sample.Z = static_cast<int16_t>(static_cast<uint16_t>(Data[4]) |
            static_cast<uint16_t>(Data[5]) << 8);
        return true;

    };

    bool MagnetometerDriver::IsAvailable() const {

        return this->Available;

    };

    uint8_t MagnetometerDriver::GetIdentity() const {

        return this->Identity;

    };

};
