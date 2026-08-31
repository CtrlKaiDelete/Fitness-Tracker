#include <cstdint>

namespace {

    static constexpr uintptr_t Port0OutputSet = 0x50000508;
    static constexpr uintptr_t Port0OutputClear = 0x5000050C;
    static constexpr uintptr_t Port0DirectionSet = 0x50000518;
    static constexpr uintptr_t Port1OutputSet = 0x50000808;
    static constexpr uintptr_t Port1OutputClear = 0x5000080C;
    static constexpr uintptr_t Port1DirectionSet = 0x50000818;
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
    static constexpr uint32_t PwmClock = 1000000;
    static constexpr uint32_t DisconnectedOutput = 0xFFFFFFFF;

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

    static constexpr uint8_t DigitFont[10][5] = {
        {0b11, 0b10, 0b10, 0b10, 0b11},
        {0b01, 0b01, 0b01, 0b01, 0b01},
        {0b11, 0b01, 0b11, 0b10, 0b11},
        {0b11, 0b01, 0b11, 0b01, 0b11},
        {0b10, 0b10, 0b11, 0b01, 0b01},
        {0b11, 0b10, 0b11, 0b01, 0b11},
        {0b11, 0b10, 0b11, 0b10, 0b11},
        {0b11, 0b01, 0b01, 0b01, 0b01},
        {0b11, 0b10, 0b11, 0b10, 0b11},
        {0b11, 0b10, 0b11, 0b01, 0b11}
    };

    inline void Write(uintptr_t Address, uint32_t Value) {
        *reinterpret_cast<volatile uint32_t*>(Address) = Value;
    }

    inline uint32_t Read(uintptr_t Address) {
        return *reinterpret_cast<volatile uint32_t*>(Address);
    }

    int32_t Abs(int32_t Value) {
        return Value < 0 ? -Value : Value;
    }

    uint16_t ToneDuty = 0;

    void StopTone() {

        Write(Pwm0StoppedEvent, 0);
        Write(Pwm0Stop, 1);
        Write(Pwm0Enable, 0);
        Write(Pwm0StoppedEvent, 0);
        Write(Pwm0SequenceEndEvent, 0);
        Write(Port0OutputClear, 1u << SpeakerPin);

    }

    void StartTone(uint32_t Frequency) {

        uint32_t Top = PwmClock / Frequency;
        ToneDuty = static_cast<uint16_t>(Top / 4);
        if(ToneDuty == 0) {
            ToneDuty = 1;
        }

        Write(Port0DirectionSet, 1u << SpeakerPin);
        StopTone();
        Write(Pwm0InterruptClear, 0xFFFFFFFF);
        Write(Pwm0Shorts, 0);
        Write(Pwm0Mode, 0);
        Write(Pwm0Prescaler, 4);
        Write(Pwm0Decoder, 0);
        Write(Pwm0Loop, 0);
        Write(Pwm0CounterTop, Top);
        Write(Pwm0SequencePointer, reinterpret_cast<uintptr_t>(&ToneDuty));
        Write(Pwm0SequenceCount, 1);
        Write(Pwm0SequenceRefresh, 0);
        Write(Pwm0SequenceEndDelay, 0);
        Write(Pwm0OutputSelect0, SpeakerPin);
        Write(Pwm0OutputSelect1, DisconnectedOutput);
        Write(Pwm0OutputSelect2, DisconnectedOutput);
        Write(Pwm0OutputSelect3, DisconnectedOutput);
        Write(Pwm0SequenceEndEvent, 0);
        Write(Pwm0Enable, 1);
        Write(Pwm0SequenceStart, 1);

    }

    struct AccelerationSample {
        int16_t X = 0;
        int16_t Y = 0;
        int16_t Z = 0;
    };

    class Accelerometer {

    public:

        void Start() {

            Write(I2cEnable, 0);
            Write(I2cClockPinSelect, 8);
            Write(I2cDataPinSelect, 16);
            Write(I2cFrequency, 0x06400000);
            Write(I2cEnable, 6);

            uint8_t Identity = 0;
            Available = ReadRegisters(0x0F, &Identity, 1) && Identity == 0x33;
            if(!Available) {
                return;
            }

            Available = WriteRegister(0x20, 0x47) && WriteRegister(0x23, 0x88);
            if(Available) {
                AccelerationSample Sample = {};
                if(ReadAcceleration(Sample)) {
                    BaseX = Sample.X;
                    BaseY = Sample.Y;
                    BaseZ = Sample.Z;
                }
            }

        }

        uint8_t ReadRunningSpeed() {

            AccelerationSample Sample = {};
            if(!Available || !ReadAcceleration(Sample)) {
                return 0;
            }

            if(ReadsSinceLastStep < MaximumStepGapReads) {
                ReadsSinceLastStep++;
            }

            int32_t Movement = Abs(Sample.X - BaseX) + Abs(Sample.Y - BaseY) + Abs(Sample.Z - BaseZ);
            if(Cooldown > 0) {
                Cooldown--;
            } else if(Movement > StepThreshold) {
                if(ReadsSinceLastStep > 0 && ReadsSinceLastStep < StoppedGapReads) {
                    uint8_t InstantSpeed = static_cast<uint8_t>(36u / ReadsSinceLastStep);
                    if(InstantSpeed > 99) {
                        InstantSpeed = 99;
                    }

                    Speed = static_cast<uint8_t>(((static_cast<uint16_t>(Speed) * 2u) + InstantSpeed) / 3u);
                }

                ReadsSinceLastStep = 0;
                Cooldown = StepCooldownReads;
            }

            if(ReadsSinceLastStep >= StoppedGapReads) {
                Speed = 0;
            }

            BaseX = static_cast<int16_t>((static_cast<int32_t>(BaseX) * 7 + Sample.X) / 8);
            BaseY = static_cast<int16_t>((static_cast<int32_t>(BaseY) * 7 + Sample.Y) / 8);
            BaseZ = static_cast<int16_t>((static_cast<int32_t>(BaseZ) * 7 + Sample.Z) / 8);
            return Speed;

        }

    private:

        bool WriteRegister(uint8_t Register, uint8_t Value) {

            uint8_t Data[2] = {Register, Value};
            return Transmit(0x19, Data, 2);

        }

        bool ReadRegisters(uint8_t Register, uint8_t* Data, uint32_t Size) {

            uint8_t Address = Size > 1 ? static_cast<uint8_t>(Register | 0x80) : Register;
            return WriteRead(0x19, &Address, 1, Data, Size);

        }

        bool ReadAcceleration(AccelerationSample& Sample) {

            uint8_t Data[6] = {};
            if(!ReadRegisters(0x28, Data, 6)) {
                return false;
            }

            Sample.X = static_cast<int16_t>(static_cast<uint16_t>(Data[0]) |
                static_cast<uint16_t>(Data[1]) << 8) >> 4;
            Sample.Y = static_cast<int16_t>(static_cast<uint16_t>(Data[2]) |
                static_cast<uint16_t>(Data[3]) << 8) >> 4;
            Sample.Z = static_cast<int16_t>(static_cast<uint16_t>(Data[4]) |
                static_cast<uint16_t>(Data[5]) << 8) >> 4;
            return true;

        }

        bool WaitForTransaction() {

            for(uint32_t Timeout = 0; Timeout < 1000000; Timeout++) {
                if(Read(I2cStoppedEvent) != 0) {
                    return Read(I2cErrorEvent) == 0;
                }

                if(Read(I2cErrorEvent) != 0) {
                    Write(I2cStop, 1);
                }
            }

            return false;

        }

        bool Transmit(uint8_t Address, const uint8_t* Data, uint32_t Size) {

            Write(I2cStoppedEvent, 0);
            Write(I2cErrorEvent, 0);
            Write(I2cErrorSource, 0xFFFFFFFF);
            Write(I2cShortcuts, 1u << 9);
            Write(I2cTransmitPointer, reinterpret_cast<uintptr_t>(Data));
            Write(I2cTransmitMaximum, Size);
            Write(I2cAddress, Address);
            Write(I2cStartTransmit, 1);
            return WaitForTransaction();

        }

        bool WriteRead(uint8_t Address, const uint8_t* WriteData, uint32_t WriteSize, uint8_t* ReadData, uint32_t ReadSize) {

            Write(I2cStoppedEvent, 0);
            Write(I2cErrorEvent, 0);
            Write(I2cErrorSource, 0xFFFFFFFF);
            Write(I2cShortcuts, (1u << 7) | (1u << 12));
            Write(I2cTransmitPointer, reinterpret_cast<uintptr_t>(WriteData));
            Write(I2cTransmitMaximum, WriteSize);
            Write(I2cReceivePointer, reinterpret_cast<uintptr_t>(ReadData));
            Write(I2cReceiveMaximum, ReadSize);
            Write(I2cAddress, Address);
            Write(I2cStartTransmit, 1);
            return WaitForTransaction();

        }

        bool Available = false;
        int16_t BaseX = 0;
        int16_t BaseY = 0;
        int16_t BaseZ = 0;
        static constexpr int32_t StepThreshold = 220;
        static constexpr uint8_t StepCooldownReads = 4;
        static constexpr uint8_t StoppedGapReads = 20;
        static constexpr uint8_t MaximumStepGapReads = 60;

        uint8_t Speed = 0;
        uint8_t Cooldown = 0;
        uint8_t ReadsSinceLastStep = MaximumStepGapReads;

    };

    class Display {

    public:

        Display() {

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

        void ShowFrame(const uint8_t Frame[5]) {

            for(int Row = 0; Row < 5; Row++) {
                Framebuffer[Row] = Frame[Row];
            }

        }

        void ShowSpeed(uint8_t Speed) {

            uint8_t Left = Speed / 10;
            uint8_t Right = Speed % 10;

            for(int Row = 0; Row < 5; Row++) {
                uint8_t LeftBits = DigitFont[Left][Row];
                uint8_t RightBits = DigitFont[Right][Row];
                Framebuffer[Row] = static_cast<uint8_t>(LeftBits | (RightBits << 3));
            }

        }

        void BootAnimation() {

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

        void Hold(uint32_t Milliseconds) {

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

    private:

        void Scan(int Row) {

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

        void WaitForTick() {

            volatile uint32_t* Control = reinterpret_cast<volatile uint32_t*>(SysTickControl);
            while((*Control & (1u << 16)) == 0) {
            }

        }

        uint8_t Framebuffer[5] = {};

    };

}

extern "C" int main() {

    Display Screen;
    Accelerometer Motion;

    Screen.BootAnimation();
    Motion.Start();

    while(true) {

        Screen.ShowSpeed(Motion.ReadRunningSpeed());
        Screen.Hold(100);

    }

}
