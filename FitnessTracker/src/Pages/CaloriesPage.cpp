#include "Pages/CaloriesPage.hpp"

#include "Accelerometer.hpp"
#include "Display.hpp"

namespace FitnessTracker {

    void CaloriesPage::Show(Display& Screen, Accelerometer& Motion) {
        Screen.ShowSpeed(Motion.GetCalories());
    }

}
