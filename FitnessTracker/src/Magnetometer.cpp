#include "Magnetometer.hpp"

#include "Hardware.hpp"
#include "Math.hpp"

namespace FitnessTracker {

    void Magnetometer::Start() {

        Write(I2cEnable, 0);
        Write(I2cClockPinSelect, 8);
        Write(I2cDataPinSelect, 16);
        Write(I2cFrequency, 0x06400000);
        Write(I2cEnable, 6);

        uint8_t Identity = 0;
        Available = ReadRegisters(0x4F, &Identity, 1) && Identity == 0x40;
        if(!Available) {
            return;
        }

        Available = WriteRegister(0x60, 0x80) && WriteRegister(0x62, 0x10);

    }

    uint8_t Magnetometer::ReadHeading() {

        MagneticFieldSample Sample = {};
        if(!Available || !ReadMagneticField(Sample)) {
            return Heading;
        }

        if(Sample.X < MinimumX) {
            MinimumX = Sample.X;
        }

        if(Sample.X > MaximumX) {
            MaximumX = Sample.X;
        }

        if(Sample.Y < MinimumY) {
            MinimumY = Sample.Y;
        }

        if(Sample.Y > MaximumY) {
            MaximumY = Sample.Y;
        }

        int32_t X = Sample.X;
        int32_t Y = Sample.Y;
        if(MaximumX - MinimumX >= 100 && MaximumY - MinimumY >= 100) {
            X -= (static_cast<int32_t>(MinimumX) + MaximumX) / 2;
            Y -= (static_cast<int32_t>(MinimumY) + MaximumY) / 2;
        }

        if(X != 0 || Y != 0) {
            Heading = GetHeading(X, Y);
        }

        return Heading;

    }

    uint8_t Magnetometer::GetHeading(int32_t X, int32_t Y) {

        int32_t AbsoluteX = Abs(X);
        int32_t AbsoluteY = Abs(Y);
        bool MostlyVertical = static_cast<int64_t>(AbsoluteX) * 1000 <=
            static_cast<int64_t>(AbsoluteY) * 414;
        bool MostlyHorizontal = static_cast<int64_t>(AbsoluteY) * 1000 <=
            static_cast<int64_t>(AbsoluteX) * 414;

        if(MostlyVertical) {
            return Y < 0 ? 0 : 4;
        }

        if(MostlyHorizontal) {
            return X > 0 ? 2 : 6;
        }

        if(X > 0) {
            return Y < 0 ? 1 : 3;
        }

        return Y > 0 ? 5 : 7;

    }

    bool Magnetometer::WriteRegister(uint8_t Register, uint8_t Value) {

        uint8_t Data[2] = {Register, Value};
        return Transmit(0x1E, Data, 2);

    }

    bool Magnetometer::ReadRegisters(uint8_t Register, uint8_t* Data, uint32_t Size) {
        return WriteRead(0x1E, &Register, 1, Data, Size);
    }

    bool Magnetometer::ReadMagneticField(MagneticFieldSample& Sample) {

        uint8_t Data[6] = {};
        if(!ReadRegisters(0x68, Data, 6)) {
            return false;
        }

        Sample.X = static_cast<int16_t>(static_cast<uint16_t>(Data[0]) |
            static_cast<uint16_t>(Data[1]) << 8);
        Sample.Y = static_cast<int16_t>(static_cast<uint16_t>(Data[2]) |
            static_cast<uint16_t>(Data[3]) << 8);
        Sample.Z = static_cast<int16_t>(static_cast<uint16_t>(Data[4]) |
            static_cast<uint16_t>(Data[5]) << 8);
        return true;

    }

    bool Magnetometer::WaitForTransaction() {

        for(uint32_t Timeout = 0; Timeout < 1000000; Timeout++) {
            if(Read(I2cStoppedEvent) != 0) {
                return Read(I2cErrorEvent) == 0;
            }

            if(Read(I2cErrorEvent) != 0) {
                Write(I2cStop, 1);
            }
        }

        return false;

    }

    bool Magnetometer::Transmit(uint8_t Address, const uint8_t* Data, uint32_t Size) {

        Write(I2cStoppedEvent, 0);
        Write(I2cErrorEvent, 0);
        Write(I2cErrorSource, 0xFFFFFFFF);
        Write(I2cShortcuts, 1u << 9);
        Write(I2cTransmitPointer, reinterpret_cast<uintptr_t>(Data));
        Write(I2cTransmitMaximum, Size);
        Write(I2cAddress, Address);
        Write(I2cStartTransmit, 1);
        return WaitForTransaction();

    }

    bool Magnetometer::WriteRead(uint8_t Address, const uint8_t* WriteData, uint32_t WriteSize, uint8_t* ReadData, uint32_t ReadSize) {

        Write(I2cStoppedEvent, 0);
        Write(I2cErrorEvent, 0);
        Write(I2cErrorSource, 0xFFFFFFFF);
        Write(I2cShortcuts, (1u << 7) | (1u << 12));
        Write(I2cTransmitPointer, reinterpret_cast<uintptr_t>(WriteData));
        Write(I2cTransmitMaximum, WriteSize);
        Write(I2cReceivePointer, reinterpret_cast<uintptr_t>(ReadData));
        Write(I2cReceiveMaximum, ReadSize);
        Write(I2cAddress, Address);
        Write(I2cStartTransmit, 1);
        return WaitForTransaction();

    }

}
