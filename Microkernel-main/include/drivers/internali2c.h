#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    class InternalI2CDriver : public Driver {

    public:

        static constexpr uintptr_t Peripheral = 0x40004000;

        bool Transmit(uint8_t Address, const uint8_t* Data, size_t Size);
        bool WriteRead(
            uint8_t Address,
            const uint8_t* WriteData,
            size_t WriteSize,
            uint8_t* ReadData,
            size_t ReadSize
        );

    protected:

        void OnStart() override;
        void OnStop() override;
        void OnPanic() override;

    private:

        bool WaitForTransaction();
        void Stop();

        static constexpr uint32_t SCLPin = 8;
        static constexpr uint32_t SDAPin = 16;

    };

};
