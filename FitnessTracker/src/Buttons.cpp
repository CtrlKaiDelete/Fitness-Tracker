#include "Buttons.hpp"

#include "Hardware.hpp"

namespace FitnessTracker {

    void Buttons::Start() {

        Write(Port0DirectionClear, (1u << ButtonAPin) | (1u << ButtonBPin));
        Write(Port0PinConfigButtonA, InputConnectedPullUp);
        Write(Port0PinConfigButtonB, InputConnectedPullUp);
        PreviousA = IsDown(ButtonAPin);
        PreviousB = IsDown(ButtonBPin);

    }

    bool Buttons::APressed() {
        return Pressed(ButtonAPin, PreviousA);
    }

    bool Buttons::BPressed() {
        return Pressed(ButtonBPin, PreviousB);
    }

    bool Buttons::ADown() {
        return IsDown(ButtonAPin);
    }

    bool Buttons::BDown() {
        return IsDown(ButtonBPin);
    }

    bool Buttons::BothDown() {
        return ADown() && BDown();
    }

    bool Buttons::IsDown(uint32_t Pin) {
        return (Read(Port0Input) & (1u << Pin)) == 0;
    }

    bool Buttons::Pressed(uint32_t Pin, bool& Previous) {

        bool Current = IsDown(Pin);
        bool Started = Current && !Previous;
        Previous = Current;
        return Started;

    }

}
