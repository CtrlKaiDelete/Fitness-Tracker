#pragma once

#include <cstdint>

namespace FitnessTracker {

    class Display;
    class Magnetometer;

    class CompassPage {

    public:

        void Show(Display& Screen, Magnetometer& Compass);

    private:

        uint16_t Frame = 0;

    };

}
