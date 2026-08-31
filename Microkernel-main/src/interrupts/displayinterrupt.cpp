#include "Microkernel/include/interrupts/displayinterrupt.h"
#include "Microkernel/include/drivers/display.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    DisplayInterrupt::DisplayInterrupt() {

        this->InterruptNumber = 9;
        this->Display = &this->Kernel->DeviceManager.LoadDriver<DisplayDriver>();

        *reinterpret_cast<volatile uint32_t*>(0xE000E180) = 1u << 9;
        *reinterpret_cast<volatile uint32_t*>(0x40009004) = 1;
        *reinterpret_cast<volatile uint32_t*>(0x40009308) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(0x40009140) = 0;
        *reinterpret_cast<volatile uint32_t*>(0x40009504) = 0;
        *reinterpret_cast<volatile uint32_t*>(0x40009508) = 3;
        *reinterpret_cast<volatile uint32_t*>(0x40009510) = 4;
        *reinterpret_cast<volatile uint32_t*>(0x40009540) = 125;
        *reinterpret_cast<volatile uint32_t*>(0x40009200) = 1;
        *reinterpret_cast<volatile uint32_t*>(0x4000900C) = 1;
        *reinterpret_cast<volatile uint32_t*>(0x40009304) = 1u << 16;
        *reinterpret_cast<volatile uint32_t*>(0xE000E280) = 1u << 9;
        *reinterpret_cast<volatile uint8_t*>(0xE000E400 + 9) = 0x40;
        *reinterpret_cast<volatile uint32_t*>(0x40009000) = 1;

    };

    void DisplayInterrupt::Execute() {

        volatile uint32_t* Event = reinterpret_cast<volatile uint32_t*>(0x40009140);
        if(*Event == 0) {

            return;

        };

        *Event = 0;
        (void)*Event;
        if(this->Display != nullptr) {

            this->Display->Scan();

        };

    };

};
