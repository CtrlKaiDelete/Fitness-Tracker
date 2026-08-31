#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    struct AccelerationSample {

        int16_t X = 0;
        int16_t Y = 0;
        int16_t Z = 0;

    };

    class AccelerometerDriver : public Driver {

    public:

        bool ReadAcceleration(AccelerationSample& Sample);
        bool IsAvailable() const;
        uint8_t GetIdentity() const;

    protected:

        void OnStart() override;

    private:

        bool WriteRegister(uint8_t Register, uint8_t Value);
        bool ReadRegisters(uint8_t Register, uint8_t* Data, size_t Size);

        static constexpr uint8_t Address = 0x19;
        static constexpr uint8_t ExpectedIdentity = 0x33;

        uint8_t Identity = 0;
        bool Available = false;

    };

};
