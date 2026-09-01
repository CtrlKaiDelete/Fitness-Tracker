#include "Pages/CompassPage.hpp"

#include "Display.hpp"
#include "Magnetometer.hpp"

namespace FitnessTracker {

    void CompassPage::Show(Display& Screen, Magnetometer& Compass) {

        Screen.ShowCompass(Compass.ReadHeading(), Frame / 8);
        Frame++;

    }

}
