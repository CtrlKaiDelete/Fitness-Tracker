#pragma once

#include <cstdint>

namespace FitnessTracker {

    struct MagneticFieldSample {
        int16_t X = 0;
        int16_t Y = 0;
        int16_t Z = 0;
    };

    class Magnetometer {

    public:

        void Start();
        uint8_t ReadHeading();

    private:

        static uint8_t GetHeading(int32_t X, int32_t Y);
        bool WriteRegister(uint8_t Register, uint8_t Value);
        bool ReadRegisters(uint8_t Register, uint8_t* Data, uint32_t Size);
        bool ReadMagneticField(MagneticFieldSample& Sample);
        bool WaitForTransaction();
        bool Transmit(uint8_t Address, const uint8_t* Data, uint32_t Size);
        bool WriteRead(uint8_t Address, const uint8_t* WriteData, uint32_t WriteSize, uint8_t* ReadData, uint32_t ReadSize);

        bool Available = false;
        int16_t MinimumX = 32767;
        int16_t MaximumX = -32768;
        int16_t MinimumY = 32767;
        int16_t MaximumY = -32768;
        uint8_t Heading = 0;

    };

}
