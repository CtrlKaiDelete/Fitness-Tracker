#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstdint>

namespace Microkernel {

    class GPIO;
    class DisplayDriver : public Driver {

    public:

        void Scan();
        void WritePixel(int X, int Y, float Brightness);

    protected:

        void OnStart() override;

        int RowToScan = 0;
        uint8_t BrightnessPhase = 0;
        uint8_t Framebuffer[5][5] = {};
        GPIO* GPIODriver = nullptr;

        static constexpr int RowPorts[5] = {

            0,0,0,0,0

        };

        static constexpr int ColumnPorts[5] = {

            /*
             * I dont know why, I dont want to know why, but
             * whatever idiot made this designed it so that
             * column 3 lives on port 1...
            */

            0,0,0,1,0

        };

        static constexpr int RowPins[5] = {

            21,22,15,24,19

        };

        static constexpr int ColumnPins[5] = {

            28,11,31,5,30

        };

    };

};
