#pragma once

#include <cstdint>

namespace FitnessTracker {

    class Display {

    public:

        Display();

        void ShowFrame(const uint8_t Frame[5]);
        void ShowSpeed(uint8_t Speed);
        void ShowPlaceholder();
        void ShowCompass(uint8_t Heading, uint16_t Frame);
        void BootAnimation();
        void Hold(uint32_t Milliseconds);

    private:

        void SetPixel(int X, int Y);
        static uint8_t DigitRowToScreenBits(uint8_t Bits, uint8_t Offset);
        void ShowCompassArrow(uint8_t Heading);
        void ShowDirectionLabel(uint8_t Heading);
        void Scan(int Row);
        void WaitForTick();

        uint8_t Framebuffer[5] = {};

    };

}
