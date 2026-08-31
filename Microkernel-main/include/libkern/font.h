#pragma once

#include <cstdint>

namespace Microkernel {

    class Font {

    public:

        Font() = default;
        virtual ~Font() = default;

        uint8_t GetRow(char Character, int Row) const;

        static constexpr int Width = 5;
        static constexpr int Height = 5;

    };

};
