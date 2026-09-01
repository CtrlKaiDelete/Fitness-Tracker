#include "Math.hpp"

namespace FitnessTracker {

    int32_t Abs(int32_t Value) {
        return Value < 0 ? -Value : Value;
    }

    uint32_t IntegerSquareRoot(uint64_t Value) {

        uint64_t Result = 0;
        uint64_t Bit = static_cast<uint64_t>(1) << 62;
        while(Bit > Value) {
            Bit >>= 2;
        }

        while(Bit != 0) {
            if(Value >= Result + Bit) {
                Value -= Result + Bit;
                Result = (Result >> 1) + Bit;
            } else {
                Result >>= 1;
            }

            Bit >>= 2;
        }

        return static_cast<uint32_t>(Result);

    }

    uint32_t GetMagnitude(int16_t X, int16_t Y, int16_t Z) {

        int32_t WideX = X;
        int32_t WideY = Y;
        int32_t WideZ = Z;
        uint64_t Squared =
            static_cast<uint64_t>(WideX * WideX) +
            static_cast<uint64_t>(WideY * WideY) +
            static_cast<uint64_t>(WideZ * WideZ);
        return IntegerSquareRoot(Squared);

    }

}
