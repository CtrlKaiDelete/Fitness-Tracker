#pragma once

#include <cstdint>

namespace FitnessTracker {

    class Display;

    class StopwatchPage {

    public:

        void Reset(uint32_t CurrentTick);
        void Show(Display& Screen, uint32_t CurrentTick);

    private:

        uint32_t StartTick = 0;

    };

}
