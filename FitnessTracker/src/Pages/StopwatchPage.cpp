#include "Pages/StopwatchPage.hpp"

#include "Display.hpp"

namespace FitnessTracker {

    void StopwatchPage::Reset(uint32_t CurrentTick) {
        StartTick = CurrentTick;
    }

    void StopwatchPage::Show(Display& Screen, uint32_t CurrentTick) {

        uint32_t Seconds = ((CurrentTick - StartTick) / 1000) % 100;
        Screen.ShowSpeed(static_cast<uint8_t>(Seconds));

    }

}
