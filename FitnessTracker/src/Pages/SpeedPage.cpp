#include "Pages/SpeedPage.hpp"

#include "Accelerometer.hpp"
#include "Display.hpp"

namespace FitnessTracker {

    void SpeedPage::Show(Display& Screen, Accelerometer& Motion, uint32_t CurrentTick) {
        Screen.ShowSpeed(Motion.ReadRunningSpeed(CurrentTick));
    }

}
