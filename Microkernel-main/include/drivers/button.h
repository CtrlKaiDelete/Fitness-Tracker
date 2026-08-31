#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    enum class Button {

        A,
        B

    };

    class ButtonDriver : public Driver {

    public:

        ButtonDriver() = default;
        virtual ~ButtonDriver() = default;

        bool IsPressed(Button Button);
        bool IsHeld(Button Button);

    protected:

        void OnStart() override;

        static constexpr int ButtonPort = 0;
        static constexpr int ButtonAPin = 14;
        static constexpr int ButtonBPin = 23;
        static constexpr uint64_t HoldDebounceMilliseconds = 20;

        struct ButtonState {

            uint64_t CandidateSince = 0;
            bool CandidatePressed = false;
            bool Held = false;

        };

        ButtonState States[2] = {};

    };

};
