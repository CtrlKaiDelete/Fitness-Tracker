#pragma once

#include <cstdint>

namespace FitnessTracker {

    class Accelerometer;
    class Display;

    class SpeedPage {

    public:

        void Show(Display& Screen, Accelerometer& Motion, uint32_t CurrentTick);

    };

}
