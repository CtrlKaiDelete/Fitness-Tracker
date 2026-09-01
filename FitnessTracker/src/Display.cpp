#include "Display.hpp"

#include "Audio.hpp"
#include "Hardware.hpp"

namespace FitnessTracker {

    namespace {

        static constexpr uint8_t Blank[5] = {
            0b00000,
            0b00000,
            0b00000,
            0b00000,
            0b00000
        };

        static constexpr uint8_t AllOn[5] = {
            0b11111,
            0b11111,
            0b11111,
            0b11111,
            0b11111
        };

        static constexpr uint8_t BootCenter[5] = {
            0b00000,
            0b00000,
            0b00100,
            0b00000,
            0b00000
        };

        static constexpr uint8_t BootSquare[5] = {
            0b00000,
            0b01110,
            0b01110,
            0b01110,
            0b00000
        };

        static constexpr uint8_t PlaceholderDot[5] = {
            0b00000,
            0b00000,
            0b00100,
            0b00000,
            0b00000
        };

        static constexpr uint8_t DigitFont[10][5] = {
            {0b11, 0b10, 0b10, 0b10, 0b11},
            {0b01, 0b11, 0b01, 0b01, 0b11},
            {0b11, 0b01, 0b11, 0b10, 0b11},
            {0b11, 0b01, 0b11, 0b01, 0b11},
            {0b10, 0b10, 0b11, 0b01, 0b01},
            {0b11, 0b10, 0b11, 0b01, 0b11},
            {0b10, 0b10, 0b11, 0b11, 0b11},
            {0b11, 0b01, 0b01, 0b01, 0b01},
            {0b11, 0b11, 0b11, 0b11, 0b11},
            {0b11, 0b11, 0b11, 0b01, 0b01}
        };

    }

    Display::Display() {

        Write(Timer0Stop, 1);
        Write(Timer1Stop, 1);
        Write(Timer2Stop, 1);
        Write(Pwm0Stop, 1);
        Write(Pwm1Stop, 1);
        Write(Pwm2Stop, 1);
        Write(SysTickControl, 0);

        Write(Port0DirectionSet, MatrixPort0Mask);
        Write(Port1DirectionSet, ColumnPort1Mask);

        Write(SysTickReload, 63999);
        Write(SysTickCurrent, 0);
        Write(SysTickControl, 5);

        ShowFrame(Blank);

    }

    void Display::ShowFrame(const uint8_t Frame[5]) {

        for(int Row = 0; Row < 5; Row++) {
            Framebuffer[Row] = Frame[Row];
        }

    }

    void Display::ShowSpeed(uint8_t Speed) {

        uint8_t Left = Speed / 10;
        uint8_t Right = Speed % 10;

        for(int Row = 0; Row < 5; Row++) {
            Framebuffer[Row] = static_cast<uint8_t>(
                DigitRowToScreenBits(DigitFont[Left][Row], 0) |
                DigitRowToScreenBits(DigitFont[Right][Row], 3)
            );
        }

    }

    void Display::ShowPlaceholder() {
        ShowFrame(PlaceholderDot);
    }

    void Display::ShowCompass(uint8_t Heading, uint16_t Frame) {

        if(Frame % 6 >= 4) {
            ShowDirectionLabel(Heading);
        } else {
            ShowCompassArrow(Heading);
        }

    }

    void Display::BootAnimation() {

        StartTone(330);
        ShowFrame(BootCenter);
        Hold(300);
        StopTone();

        ShowFrame(Blank);
        Hold(120);

        StartTone(660);
        ShowFrame(BootSquare);
        Hold(300);
        StopTone();

        ShowFrame(Blank);
        Hold(120);

        StartTone(990);
        ShowFrame(AllOn);
        Hold(350);
        StopTone();

    }

    void Display::Hold(uint32_t Milliseconds) {

        int Row = 0;
        for(uint32_t Millisecond = 0; Millisecond < Milliseconds; Millisecond++) {

            Scan(Row);
            WaitForTick();

            Row++;
            if(Row >= 5) {
                Row = 0;
            }

        }

    }

    void Display::SetPixel(int X, int Y) {

        if(X < 0 || X >= 5 || Y < 0 || Y >= 5) {
            return;
        }

        Framebuffer[Y] |= static_cast<uint8_t>(1u << X);

    }

    uint8_t Display::DigitRowToScreenBits(uint8_t Bits, uint8_t Offset) {

        uint8_t Row = 0;
        if((Bits & 0b10) != 0) {
            Row |= static_cast<uint8_t>(1u << Offset);
        }

        if((Bits & 0b01) != 0) {
            Row |= static_cast<uint8_t>(1u << (Offset + 1));
        }

        return Row;

    }

    void Display::ShowCompassArrow(uint8_t Heading) {

        static constexpr int Pixels[8][5][2] = {
            {{2, 2}, {2, 1}, {2, 0}, {1, 1}, {3, 1}},
            {{2, 2}, {3, 1}, {4, 0}, {3, 0}, {4, 1}},
            {{2, 2}, {3, 2}, {4, 2}, {3, 1}, {3, 3}},
            {{2, 2}, {3, 3}, {4, 4}, {3, 4}, {4, 3}},
            {{2, 2}, {2, 3}, {2, 4}, {1, 3}, {3, 3}},
            {{2, 2}, {1, 3}, {0, 4}, {1, 4}, {0, 3}},
            {{2, 2}, {1, 2}, {0, 2}, {1, 1}, {1, 3}},
            {{2, 2}, {1, 1}, {0, 0}, {1, 0}, {0, 1}}
        };

        ShowFrame(Blank);
        for(const auto& Pixel : Pixels[Heading & 7]) {
            SetPixel(Pixel[0], Pixel[1]);
        }

    }

    void Display::ShowDirectionLabel(uint8_t Heading) {

        static constexpr uint8_t CardinalRows[4][5] = {
            {0b10001, 0b11001, 0b10101, 0b10011, 0b10001},
            {0b11111, 0b10000, 0b11110, 0b10000, 0b11111},
            {0b01111, 0b10000, 0b01110, 0b00001, 0b11110},
            {0b10001, 0b10001, 0b10101, 0b11011, 0b10001}
        };

        static constexpr uint8_t EdgeLetterRows[4][5] = {
            {0b10, 0b11, 0b11, 0b11, 0b01},
            {0b11, 0b10, 0b11, 0b10, 0b11},
            {0b11, 0b10, 0b11, 0b01, 0b11},
            {0b10, 0b10, 0b11, 0b11, 0b11}
        };

        Heading &= 7;
        if((Heading & 1u) == 0) {
            uint8_t Cardinal = (Heading + 1) / 2 % 4;
            ShowFrame(CardinalRows[Cardinal]);
            return;
        }

        uint8_t First = Heading < 4 ? 0 : 2;
        uint8_t Second = Heading == 1 || Heading == 3 ? 1 : 3;
        for(int Row = 0; Row < 5; Row++) {
            Framebuffer[Row] = static_cast<uint8_t>(
                EdgeLetterRows[First][Row] | (EdgeLetterRows[Second][Row] << 3)
            );
        }

    }

    void Display::Scan(int Row) {

        Write(Port0OutputClear, RowMask);
        Write(Port0OutputSet, ColumnPort0Mask);
        Write(Port1OutputSet, ColumnPort1Mask);

        uint8_t Pixels = Framebuffer[Row];
        for(int Column = 0; Column < 5; Column++) {

            if((Pixels & (1u << Column)) == 0) {
                continue;
            }

            if(ColumnPorts[Column] == 0) {
                Write(Port0OutputClear, 1u << ColumnPins[Column]);
            } else {
                Write(Port1OutputClear, 1u << ColumnPins[Column]);
            }

        }

        Write(Port0OutputSet, 1u << RowPins[Row]);

    }

    void Display::WaitForTick() {

        volatile uint32_t* Control = reinterpret_cast<volatile uint32_t*>(SysTickControl);
        while((*Control & (1u << 16)) == 0) {
        }

    }

}
