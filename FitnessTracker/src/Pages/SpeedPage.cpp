#include "Pages/SpeedPage.hpp"

#include "Accelerometer.hpp"
#include "Display.hpp"

namespace FitnessTracker {

    void SpeedPage::Show(Display& Screen, Accelerometer& Motion) {
        Screen.ShowSpeed(Motion.GetRunningSpeed());
    }

}
