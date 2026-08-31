#include "Microkernel/include/drivers/button.h"
#include "Microkernel/include/drivers/gpio.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    void ButtonDriver::OnStart() {

        auto& GPIODriver = this->Kernel->DeviceManager.LoadDriver<GPIO>();

        GPIODriver.SetInput(this->ButtonPort, this->ButtonAPin);
        GPIODriver.SetInput(this->ButtonPort, this->ButtonBPin);

        for(auto& State : this->States) {

            State = {};

        };

    };

    bool ButtonDriver::IsPressed(Button Button) {

        DriverWatchdogScope Watchdog(*this);
        auto& GPIODriver = this->Kernel->DeviceManager.LoadDriver<GPIO>();
        int Pin = Button == Button::A ? this->ButtonAPin : this->ButtonBPin;

        return GPIODriver.ReadPin(this->ButtonPort, Pin) == GPIOLow;

    };

    bool ButtonDriver::IsHeld(Button Button) {

        DriverWatchdogScope Watchdog(*this);

        std::size_t Index = Button == Button::A ? 0 : 1;
        ButtonState& State = this->States[Index];
        bool Pressed = this->IsPressed(Button);
        uint64_t CurrentTick = this->Kernel->InterruptTimer.GetTicks();

        if(Pressed != State.CandidatePressed) {

            State.CandidatePressed = Pressed;
            State.CandidateSince = CurrentTick;

        } else if(State.Held != State.CandidatePressed
            && CurrentTick - State.CandidateSince >= this->HoldDebounceMilliseconds) {

            State.Held = State.CandidatePressed;

        };

        return State.Held;

    };

};
