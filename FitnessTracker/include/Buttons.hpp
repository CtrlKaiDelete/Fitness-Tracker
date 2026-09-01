#pragma once

#include <cstdint>

namespace FitnessTracker {

    class Buttons {

    public:

        void Start();
        bool APressed();
        bool BPressed();
        bool ADown();
        bool BDown();
        bool BothDown();

    private:

        bool IsDown(uint32_t Pin);
        bool Pressed(uint32_t Pin, bool& Previous);

        bool PreviousA = false;
        bool PreviousB = false;

    };

}
