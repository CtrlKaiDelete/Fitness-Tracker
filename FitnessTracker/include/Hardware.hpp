#pragma once

#include <cstdint>

namespace FitnessTracker {

    static constexpr uintptr_t Port0OutputSet = 0x50000508;
    static constexpr uintptr_t Port0OutputClear = 0x5000050C;
    static constexpr uintptr_t Port0Input = 0x50000510;
    static constexpr uintptr_t Port0DirectionSet = 0x50000518;
    static constexpr uintptr_t Port0DirectionClear = 0x5000051C;
    static constexpr uintptr_t Port0PinConfigButtonA = 0x50000738;
    static constexpr uintptr_t Port0PinConfigButtonB = 0x5000075C;
    static constexpr uintptr_t Port1OutputSet = 0x50000808;
    static constexpr uintptr_t Port1OutputClear = 0x5000080C;
    static constexpr uintptr_t Port1Input = 0x50000810;
    static constexpr uintptr_t Port1DirectionSet = 0x50000818;
    static constexpr uintptr_t Port1DirectionClear = 0x5000081C;
    static constexpr uintptr_t Port1PinConfigLogo = 0x50000A10;
    static constexpr uintptr_t Timer0Stop = 0x40008004;
    static constexpr uintptr_t Timer1Stop = 0x40009004;
    static constexpr uintptr_t Timer2Stop = 0x4000A004;
    static constexpr uintptr_t Pwm0Stop = 0x4001C004;
    static constexpr uintptr_t Pwm0SequenceStart = 0x4001C008;
    static constexpr uintptr_t Pwm0StoppedEvent = 0x4001C104;
    static constexpr uintptr_t Pwm0SequenceEndEvent = 0x4001C110;
    static constexpr uintptr_t Pwm0Shorts = 0x4001C200;
    static constexpr uintptr_t Pwm0InterruptClear = 0x4001C308;
    static constexpr uintptr_t Pwm0Enable = 0x4001C500;
    static constexpr uintptr_t Pwm0Mode = 0x4001C504;
    static constexpr uintptr_t Pwm0CounterTop = 0x4001C508;
    static constexpr uintptr_t Pwm0Prescaler = 0x4001C50C;
    static constexpr uintptr_t Pwm0Decoder = 0x4001C510;
    static constexpr uintptr_t Pwm0Loop = 0x4001C514;
    static constexpr uintptr_t Pwm0SequencePointer = 0x4001C520;
    static constexpr uintptr_t Pwm0SequenceCount = 0x4001C524;
    static constexpr uintptr_t Pwm0SequenceRefresh = 0x4001C528;
    static constexpr uintptr_t Pwm0SequenceEndDelay = 0x4001C52C;
    static constexpr uintptr_t Pwm0OutputSelect0 = 0x4001C560;
    static constexpr uintptr_t Pwm0OutputSelect1 = 0x4001C564;
    static constexpr uintptr_t Pwm0OutputSelect2 = 0x4001C568;
    static constexpr uintptr_t Pwm0OutputSelect3 = 0x4001C56C;
    static constexpr uintptr_t Pwm1Stop = 0x40021004;
    static constexpr uintptr_t Pwm2Stop = 0x40022004;
    static constexpr uintptr_t SysTickControl = 0xE000E010;
    static constexpr uintptr_t SysTickReload = 0xE000E014;
    static constexpr uintptr_t SysTickCurrent = 0xE000E018;
    static constexpr uintptr_t DebugExceptionControl = 0xE000EDFC;
    static constexpr uintptr_t CycleCounterControl = 0xE0001000;
    static constexpr uintptr_t CycleCounter = 0xE0001004;
    static constexpr uintptr_t I2cStartTransmit = 0x40004008;
    static constexpr uintptr_t I2cStop = 0x40004014;
    static constexpr uintptr_t I2cStoppedEvent = 0x40004104;
    static constexpr uintptr_t I2cErrorEvent = 0x40004124;
    static constexpr uintptr_t I2cShortcuts = 0x40004200;
    static constexpr uintptr_t I2cErrorSource = 0x400044C4;
    static constexpr uintptr_t I2cEnable = 0x40004500;
    static constexpr uintptr_t I2cClockPinSelect = 0x40004508;
    static constexpr uintptr_t I2cDataPinSelect = 0x4000450C;
    static constexpr uintptr_t I2cFrequency = 0x40004524;
    static constexpr uintptr_t I2cReceivePointer = 0x40004534;
    static constexpr uintptr_t I2cReceiveMaximum = 0x40004538;
    static constexpr uintptr_t I2cTransmitPointer = 0x40004544;
    static constexpr uintptr_t I2cTransmitMaximum = 0x40004548;
    static constexpr uintptr_t I2cAddress = 0x40004588;
    static constexpr uint32_t RowMask = 0x01688000;
    static constexpr uint32_t ColumnPort0Mask = 0xD0000800;
    static constexpr uint32_t ColumnPort1Mask = 0x00000020;
    static constexpr uint32_t MatrixPort0Mask = 0xD1688800;
    static constexpr int RowPins[5] = {21, 22, 15, 24, 19};
    static constexpr int ColumnPorts[5] = {0, 0, 0, 1, 0};
    static constexpr int ColumnPins[5] = {28, 11, 31, 5, 30};
    static constexpr uint32_t SpeakerPin = 0;
    static constexpr uint32_t LogoPin = 4;
    static constexpr uint32_t ButtonAPin = 14;
    static constexpr uint32_t ButtonBPin = 23;
    static constexpr uint32_t InputConnectedPullUp = 0x0000000C;
    static constexpr uint32_t PwmClock = 1000000;
    static constexpr uint32_t DisconnectedOutput = 0xFFFFFFFF;

    inline void Write(uintptr_t Address, uint32_t Value) {
        *reinterpret_cast<volatile uint32_t*>(Address) = Value;
    }

    inline uint32_t Read(uintptr_t Address) {
        return *reinterpret_cast<volatile uint32_t*>(Address);
    }

}
