#include <cstdint>

namespace {

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

    static constexpr uint8_t LogoMark[5] = {
        0b00000,
        0b00000,
        0b00100,
        0b00000,
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

    inline void Write(uintptr_t Address, uint32_t Value) {
        *reinterpret_cast<volatile uint32_t*>(Address) = Value;
    }

    inline uint32_t Read(uintptr_t Address) {
        return *reinterpret_cast<volatile uint32_t*>(Address);
    }

    int32_t Abs(int32_t Value) {
        return Value < 0 ? -Value : Value;
    }

    uint32_t IntegerSquareRoot(uint64_t Value) {

        uint64_t Result = 0;
        uint64_t Bit = static_cast<uint64_t>(1) << 62;
        while(Bit > Value) {
            Bit >>= 2;
        }

        while(Bit != 0) {
            if(Value >= Result + Bit) {
                Value -= Result + Bit;
                Result = (Result >> 1) + Bit;
            } else {
                Result >>= 1;
            }

            Bit >>= 2;
        }

        return static_cast<uint32_t>(Result);

    }

    uint32_t GetMagnitude(int16_t X, int16_t Y, int16_t Z) {

        int32_t WideX = X;
        int32_t WideY = Y;
        int32_t WideZ = Z;
        uint64_t Squared =
            static_cast<uint64_t>(WideX * WideX) +
            static_cast<uint64_t>(WideY * WideY) +
            static_cast<uint64_t>(WideZ * WideZ);
        return IntegerSquareRoot(Squared);

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

    struct MagneticFieldSample {
        int16_t X = 0;
        int16_t Y = 0;
        int16_t Z = 0;
    };

    enum class ScreenMode : uint8_t {
        Speed,
        Placeholder,
        Compass
    };

    class Buttons {

    public:

        void Start() {

            Write(Port0DirectionClear, (1u << ButtonAPin) | (1u << ButtonBPin));
            Write(Port0PinConfigButtonA, InputConnectedPullUp);
            Write(Port0PinConfigButtonB, InputConnectedPullUp);
            PreviousA = IsDown(ButtonAPin);
            PreviousB = IsDown(ButtonBPin);

        }

        bool APressed() {

            return Pressed(ButtonAPin, PreviousA);

        }

        bool BPressed() {

            return Pressed(ButtonBPin, PreviousB);

        }

    private:

        bool IsDown(uint32_t Pin) {

            return (Read(Port0Input) & (1u << Pin)) == 0;

        }

        bool Pressed(uint32_t Pin, bool& Previous) {

            bool Current = IsDown(Pin);
            bool Started = Current && !Previous;
            Previous = Current;
            return Started;

        }

        bool PreviousA = false;
        bool PreviousB = false;

    };

    class LogoTouch {

    public:

        void Start() {

            Write(DebugExceptionControl, Read(DebugExceptionControl) | (1u << 24));
            Write(CycleCounter, 0);
            Write(CycleCounterControl, Read(CycleCounterControl) | 1u);
            Calibrate();

        }

        bool IsPressed() {

            uint32_t ChargeTime = ReadChargeTime();
            bool SamplePressed = ChargeTime >= Threshold;
            if(SamplePressed == Pressed) {
                SamplesMatchingNewState = 0;
            } else {
                SamplesMatchingNewState++;
                if(SamplesMatchingNewState >= PressSamples) {
                    Pressed = SamplePressed;
                    SamplesMatchingNewState = 0;
                }
            }

            return Pressed;

        }

    private:

        void Calibrate() {

            Baseline = 0;
            for(uint32_t Sample = 0; Sample < CalibrationSamples; Sample++) {
                uint32_t ChargeTime = ReadChargeTime();
                if(ChargeTime > Baseline) {
                    Baseline = ChargeTime;
                }
            }

            Threshold = Baseline + (Baseline / 2) + 32;
            if(Baseline >= MaximumChargeTime - 64) {
                Threshold = MaximumChargeTime + 1;
            } else if(Threshold > MaximumChargeTime) {
                Threshold = MaximumChargeTime + 1;
            }

            SamplesMatchingNewState = 0;
            Pressed = false;

        }

        uint32_t ReadChargeTime() {

            Write(Port1DirectionSet, 1u << LogoPin);
            Write(Port1OutputClear, 1u << LogoPin);
            for(int Cycle = 0; Cycle < 32; Cycle++) {
                __asm__ volatile("nop");
            }

            uint32_t StartCycle = Read(CycleCounter);
            Write(Port1DirectionClear, 1u << LogoPin);
            Write(Port1PinConfigLogo, Read(Port1PinConfigLogo) & ~(1u << 1));

            uint32_t ChargeTime = 0;
            while((Read(Port1Input) & (1u << LogoPin)) == 0) {
                ChargeTime = Read(CycleCounter) - StartCycle;
                if(ChargeTime >= MaximumChargeTime) {
                    ChargeTime = MaximumChargeTime;
                    break;
                }
            }

            return ChargeTime;

        }

        static constexpr uint32_t CalibrationSamples = 16;
        static constexpr uint32_t MaximumChargeTime = 64000;
        static constexpr uint32_t PressSamples = 8;

        uint32_t Baseline = 0;
        uint32_t Threshold = 0;
        uint32_t SamplesMatchingNewState = 0;
        bool Pressed = false;

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
        }

        uint8_t ReadRunningSpeed(uint32_t CurrentTick) {

            AccelerationSample Sample = {};
            if(!Available || !ReadAcceleration(Sample)) {
                return 0;
            }

            uint32_t Magnitude = GetMagnitude(Sample.X, Sample.Y, Sample.Z);
            if(!BaselineReady) {
                GravityBaseline = static_cast<int32_t>(Magnitude);
                BaselineReady = true;
                return 0;
            }

            int32_t Difference = static_cast<int32_t>(Magnitude) - GravityBaseline;
            GravityBaseline += Difference / 32;
            uint32_t MotionMagnitude = static_cast<uint32_t>(Difference < 0 ? -Difference : Difference);

            if(!AboveThreshold &&
                MotionMagnitude >= StepThresholdMilligravity &&
                CurrentTick - LastStep >= MinimumStepIntervalMilliseconds) {

                if(SeenFirstStep) {
                    uint32_t StepInterval = CurrentTick - LastStep;
                    if(StepInterval <= StoppedIntervalMilliseconds) {
                        uint32_t InstantSpeed = 18000u / StepInterval;
                        if(InstantSpeed > 99) {
                            InstantSpeed = 99;
                        }

                        Speed = static_cast<uint8_t>(((static_cast<uint16_t>(Speed) * 4u) + InstantSpeed) / 5u);
                    }
                } else {
                    SeenFirstStep = true;
                    Speed = 0;
                }

                LastStep = CurrentTick;
                AboveThreshold = true;

            } else if(AboveThreshold && MotionMagnitude <= ResetThresholdMilligravity) {

                AboveThreshold = false;

            }

            if(CurrentTick - LastStep >= StoppedIntervalMilliseconds) {
                Speed = 0;
            }

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
        static constexpr uint32_t StepThresholdMilligravity = 120;
        static constexpr uint32_t ResetThresholdMilligravity = 60;
        static constexpr uint32_t MinimumStepIntervalMilliseconds = 250;
        static constexpr uint32_t StoppedIntervalMilliseconds = 2000;

        int32_t GravityBaseline = 0;
        uint32_t LastStep = 0;
        uint8_t Speed = 0;
        bool BaselineReady = false;
        bool AboveThreshold = false;
        bool SeenFirstStep = false;

    };

    class Magnetometer {

    public:

        void Start() {

            Write(I2cEnable, 0);
            Write(I2cClockPinSelect, 8);
            Write(I2cDataPinSelect, 16);
            Write(I2cFrequency, 0x06400000);
            Write(I2cEnable, 6);

            uint8_t Identity = 0;
            Available = ReadRegisters(0x4F, &Identity, 1) && Identity == 0x40;
            if(!Available) {
                return;
            }

            Available = WriteRegister(0x60, 0x80) && WriteRegister(0x62, 0x10);

        }

        uint8_t ReadHeading() {

            MagneticFieldSample Sample = {};
            if(!Available || !ReadMagneticField(Sample)) {
                return Heading;
            }

            if(Sample.X < MinimumX) {
                MinimumX = Sample.X;
            }

            if(Sample.X > MaximumX) {
                MaximumX = Sample.X;
            }

            if(Sample.Y < MinimumY) {
                MinimumY = Sample.Y;
            }

            if(Sample.Y > MaximumY) {
                MaximumY = Sample.Y;
            }

            int32_t X = Sample.X;
            int32_t Y = Sample.Y;
            if(MaximumX - MinimumX >= 100 && MaximumY - MinimumY >= 100) {
                X -= (static_cast<int32_t>(MinimumX) + MaximumX) / 2;
                Y -= (static_cast<int32_t>(MinimumY) + MaximumY) / 2;
            }

            if(X != 0 || Y != 0) {
                Heading = GetHeading(X, Y);
            }

            return Heading;

        }

    private:

        static uint8_t GetHeading(int32_t X, int32_t Y) {

            int32_t AbsoluteX = Abs(X);
            int32_t AbsoluteY = Abs(Y);
            bool MostlyVertical = static_cast<int64_t>(AbsoluteX) * 1000 <=
                static_cast<int64_t>(AbsoluteY) * 414;
            bool MostlyHorizontal = static_cast<int64_t>(AbsoluteY) * 1000 <=
                static_cast<int64_t>(AbsoluteX) * 414;

            if(MostlyVertical) {
                return Y < 0 ? 0 : 4;
            }

            if(MostlyHorizontal) {
                return X > 0 ? 2 : 6;
            }

            if(X > 0) {
                return Y < 0 ? 1 : 3;
            }

            return Y > 0 ? 5 : 7;

        }

        bool WriteRegister(uint8_t Register, uint8_t Value) {

            uint8_t Data[2] = {Register, Value};
            return Transmit(0x1E, Data, 2);

        }

        bool ReadRegisters(uint8_t Register, uint8_t* Data, uint32_t Size) {

            return WriteRead(0x1E, &Register, 1, Data, Size);

        }

        bool ReadMagneticField(MagneticFieldSample& Sample) {

            uint8_t Data[6] = {};
            if(!ReadRegisters(0x68, Data, 6)) {
                return false;
            }

            Sample.X = static_cast<int16_t>(static_cast<uint16_t>(Data[0]) |
                static_cast<uint16_t>(Data[1]) << 8);
            Sample.Y = static_cast<int16_t>(static_cast<uint16_t>(Data[2]) |
                static_cast<uint16_t>(Data[3]) << 8);
            Sample.Z = static_cast<int16_t>(static_cast<uint16_t>(Data[4]) |
                static_cast<uint16_t>(Data[5]) << 8);
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
        int16_t MinimumX = 32767;
        int16_t MaximumX = -32768;
        int16_t MinimumY = 32767;
        int16_t MaximumY = -32768;
        uint8_t Heading = 0;

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
                Framebuffer[Row] = static_cast<uint8_t>(
                    DigitRowToScreenBits(DigitFont[Left][Row], 0) |
                    DigitRowToScreenBits(DigitFont[Right][Row], 3)
                );
            }

        }

        void ShowLogo() {

            ShowFrame(LogoMark);

        }

        void ShowPlaceholder() {

            ShowFrame(PlaceholderDot);

        }

        void ShowCompass(uint8_t Heading, uint16_t Frame) {

            if(Frame % 6 >= 4) {
                ShowDirectionLabel(Heading);
            } else {
                ShowCompassArrow(Heading);
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

        void SetPixel(int X, int Y) {

            if(X < 0 || X >= 5 || Y < 0 || Y >= 5) {
                return;
            }

            Framebuffer[Y] |= static_cast<uint8_t>(1u << X);

        }

        static uint8_t DigitRowToScreenBits(uint8_t Bits, uint8_t Offset) {

            uint8_t Row = 0;
            if((Bits & 0b10) != 0) {
                Row |= static_cast<uint8_t>(1u << Offset);
            }

            if((Bits & 0b01) != 0) {
                Row |= static_cast<uint8_t>(1u << (Offset + 1));
            }

            return Row;

        }

        void ShowCompassArrow(uint8_t Heading) {

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

        void ShowDirectionLabel(uint8_t Heading) {

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
    Magnetometer Compass;
    LogoTouch Logo;
    Buttons Controls;
    ScreenMode Mode = ScreenMode::Speed;
    bool HomeHeld = false;
    uint16_t CompassFrame = 0;
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

        if(Controls.APressed()) {
            Mode = ScreenMode::Compass;
        }

        if(Controls.BPressed()) {
            Mode = ScreenMode::Placeholder;
        }

        if(Mode == ScreenMode::Speed) {
            Screen.ShowSpeed(Motion.ReadRunningSpeed(CurrentTick));
        } else if(Mode == ScreenMode::Placeholder) {
            Screen.ShowPlaceholder();
        } else {
            Screen.ShowCompass(Compass.ReadHeading(), CompassFrame / 8);
            CompassFrame++;
        }

        Screen.Hold(100);
        CurrentTick += 100;

    }

}
