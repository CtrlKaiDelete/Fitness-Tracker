#include "Accelerometer.hpp"
#include "Buttons.hpp"
#include "Display.hpp"
#include "LogoTouch.hpp"
#include "Magnetometer.hpp"
#include "Pages.hpp"

extern "C" int main() {

    using namespace FitnessTracker;

    Display Screen;
    Accelerometer Motion;
    Magnetometer Compass;
    LogoTouch Logo;
    Buttons Controls;
    SpeedPage Speed;
    PlaceholderPage Placeholder;
    CompassPage CompassScreen;
    StopwatchPage Stopwatch;
    ScreenMode Mode = ScreenMode::Speed;
    bool HomeHeld = false;
    uint32_t BothButtonsHeldFor = 0;
    uint32_t CurrentTick = 0;

    Screen.BootAnimation();
    Motion.Start();
    Compass.Start();
    Logo.Start();
    Controls.Start();

    while(true) {

        bool HomePressed = Logo.IsPressed();
        if(HomePressed && !HomeHeld) {
            Mode = ScreenMode::Speed;
        }

        HomeHeld = HomePressed;

        if(Controls.BothDown()) {
            BothButtonsHeldFor += 100;
            if(BothButtonsHeldFor == 500) {
                Stopwatch.Reset(CurrentTick);
                Mode = ScreenMode::Stopwatch;
            }
        } else {
            BothButtonsHeldFor = 0;

            if(Controls.APressed()) {
                Mode = ScreenMode::Compass;
            }

            if(Controls.BPressed()) {
                Mode = ScreenMode::Placeholder;
            }
        }

        if(Mode == ScreenMode::Speed) {
            Speed.Show(Screen, Motion, CurrentTick);
        } else if(Mode == ScreenMode::Placeholder) {
            Placeholder.Show(Screen);
        } else if(Mode == ScreenMode::Stopwatch) {
            Stopwatch.Show(Screen, CurrentTick);
        } else {
            CompassScreen.Show(Screen, Compass);
        }

        Screen.Hold(100);
        CurrentTick += 100;

    }

}
