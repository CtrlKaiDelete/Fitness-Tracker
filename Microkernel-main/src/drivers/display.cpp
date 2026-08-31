#include "Microkernel/include/drivers/display.h"
#include "Microkernel/include/drivers/gpio.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    void DisplayDriver::OnStart() {

        this->GPIODriver = &this->Kernel->DeviceManager.LoadDriver<GPIO>();

        for(int Y = 0; Y < 5; Y++) {

            this->GPIODriver->SetOutput(this->RowPorts[Y], this->RowPins[Y]);
            this->GPIODriver->WritePin(this->RowPorts[Y], this->RowPins[Y], GPIOLow);

        };

        for(int X = 0; X < 5; X++) {

            this->GPIODriver->SetOutput(this->ColumnPorts[X], this->ColumnPins[X]);
            this->GPIODriver->WritePin(this->ColumnPorts[X], this->ColumnPins[X], GPIOHigh);

        };

    };

    void DisplayDriver::Scan() {

        DriverWatchdogScope Watchdog(*this);
        if(this->GPIODriver == nullptr) {

            return;

        };

        int PreviousRow = this->RowToScan == 0 ? 4 : this->RowToScan - 1;
        this->GPIODriver->WritePin(
            this->RowPorts[PreviousRow],
            this->RowPins[PreviousRow],
            GPIOLow
        );

        uint32_t HighMasks[2] = {};
        uint32_t LowMasks[2] = {};

        for(int X = 0; X < 5; X++) {

            int Port = this->ColumnPorts[X];
            uint32_t Mask = 1u << this->ColumnPins[X];
            if(this->Framebuffer[this->RowToScan][X] > this->BrightnessPhase) {

                LowMasks[Port] |= Mask;

            } else {

                HighMasks[Port] |= Mask;

            };

        };

        this->GPIODriver->WritePins(0, HighMasks[0], LowMasks[0]);
        this->GPIODriver->WritePins(1, HighMasks[1], LowMasks[1]);
        this->GPIODriver->WritePin(
            this->RowPorts[this->RowToScan],
            this->RowPins[this->RowToScan],
            GPIOHigh
        );

        this->RowToScan++;
        if(this->RowToScan >= 5) {

            this->RowToScan = 0;
            this->BrightnessPhase = (this->BrightnessPhase + 1) % 8;

        };

    };

    void DisplayDriver::WritePixel(int X, int Y, float Brightness) {

        DriverWatchdogScope Watchdog(*this);
        if((X < 0 || X >= 5) || (Y < 0 || Y >= 5)) {

            return;

        };

        if(Brightness > 1.0f || Brightness < 0.0f) {

            return;

        };

        this->Framebuffer[Y][X] = static_cast<uint8_t>(Brightness * 8.0f);

    };

};
