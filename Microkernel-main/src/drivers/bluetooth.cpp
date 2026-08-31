#include "Microkernel/include/drivers/bluetooth.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    namespace {

        constexpr uintptr_t Clock = 0x40000000;
        constexpr uintptr_t ClockStartHighFrequency = Clock + 0x000;
        constexpr uintptr_t ClockHighFrequencyStarted = Clock + 0x100;

        constexpr uintptr_t Radio = 0x40001000;
        constexpr uintptr_t RadioTransmitEnable = Radio + 0x000;
        constexpr uintptr_t RadioReceiveEnable = Radio + 0x004;
        constexpr uintptr_t RadioDisable = Radio + 0x010;
        constexpr uintptr_t RadioReady = Radio + 0x100;
        constexpr uintptr_t RadioAddress = Radio + 0x104;
        constexpr uintptr_t RadioEnd = Radio + 0x10C;
        constexpr uintptr_t RadioDisabled = Radio + 0x110;
        constexpr uintptr_t RadioShortcuts = Radio + 0x200;
        constexpr uintptr_t RadioInterruptClear = Radio + 0x308;
        constexpr uintptr_t RadioCRCStatus = Radio + 0x400;
        constexpr uintptr_t RadioPacketPointer = Radio + 0x504;
        constexpr uintptr_t RadioFrequency = Radio + 0x508;
        constexpr uintptr_t RadioTransmitPower = Radio + 0x50C;
        constexpr uintptr_t RadioMode = Radio + 0x510;
        constexpr uintptr_t RadioPacketConfigurationZero = Radio + 0x514;
        constexpr uintptr_t RadioPacketConfigurationOne = Radio + 0x518;
        constexpr uintptr_t RadioBaseZero = Radio + 0x51C;
        constexpr uintptr_t RadioPrefixZero = Radio + 0x524;
        constexpr uintptr_t RadioTransmitAddress = Radio + 0x52C;
        constexpr uintptr_t RadioReceiveAddresses = Radio + 0x530;
        constexpr uintptr_t RadioCRCConfiguration = Radio + 0x534;
        constexpr uintptr_t RadioCRCPolynomial = Radio + 0x538;
        constexpr uintptr_t RadioCRCInitialization = Radio + 0x53C;
        constexpr uintptr_t RadioInterframeSpacing = Radio + 0x544;
        constexpr uintptr_t RadioState = Radio + 0x550;
        constexpr uintptr_t RadioDataWhiteInitialization = Radio + 0x554;
        constexpr uintptr_t RadioModeConfiguration = Radio + 0x650;
        constexpr uintptr_t RadioPower = Radio + 0xFFC;

        constexpr uintptr_t RandomNumberGenerator = 0x4000D000;
        constexpr uintptr_t RandomNumberGeneratorStart = RandomNumberGenerator + 0x000;
        constexpr uintptr_t RandomNumberGeneratorStop = RandomNumberGenerator + 0x004;
        constexpr uintptr_t RandomNumberGeneratorValueReady = RandomNumberGenerator + 0x100;
        constexpr uintptr_t RandomNumberGeneratorConfiguration = RandomNumberGenerator + 0x504;
        constexpr uintptr_t RandomNumberGeneratorValue = RandomNumberGenerator + 0x508;

        constexpr uintptr_t ElectronicCodebook = 0x4000E000;
        constexpr uintptr_t ElectronicCodebookStart = ElectronicCodebook + 0x000;
        constexpr uintptr_t ElectronicCodebookEnd = ElectronicCodebook + 0x100;
        constexpr uintptr_t ElectronicCodebookError = ElectronicCodebook + 0x104;
        constexpr uintptr_t ElectronicCodebookDataPointer = ElectronicCodebook + 0x504;

        constexpr uintptr_t CounterWithCBCMAC = 0x4000F000;
        constexpr uintptr_t CounterWithCBCMACGenerate = CounterWithCBCMAC + 0x000;
        constexpr uintptr_t CounterWithCBCMACEndCrypt = CounterWithCBCMAC + 0x104;
        constexpr uintptr_t CounterWithCBCMACError = CounterWithCBCMAC + 0x108;
        constexpr uintptr_t CounterWithCBCMACShortcuts = CounterWithCBCMAC + 0x200;
        constexpr uintptr_t CounterWithCBCMACMICStatus = CounterWithCBCMAC + 0x400;
        constexpr uintptr_t CounterWithCBCMACEnable = CounterWithCBCMAC + 0x500;
        constexpr uintptr_t CounterWithCBCMACMode = CounterWithCBCMAC + 0x504;
        constexpr uintptr_t CounterWithCBCMACConfigurationPointer = CounterWithCBCMAC + 0x508;
        constexpr uintptr_t CounterWithCBCMACInputPointer = CounterWithCBCMAC + 0x50C;
        constexpr uintptr_t CounterWithCBCMACOutputPointer = CounterWithCBCMAC + 0x510;
        constexpr uintptr_t CounterWithCBCMACScratchPointer = CounterWithCBCMAC + 0x514;
        constexpr uintptr_t CounterWithCBCMACHeaderMask = CounterWithCBCMAC + 0x520;

        constexpr uintptr_t NonVolatileMemoryController = 0x4001E000;
        constexpr uintptr_t NonVolatileMemoryControllerReady = NonVolatileMemoryController + 0x400;
        constexpr uintptr_t NonVolatileMemoryControllerConfiguration = NonVolatileMemoryController + 0x504;
        constexpr uintptr_t NonVolatileMemoryControllerErasePage = NonVolatileMemoryController + 0x508;

        constexpr uintptr_t MemoryProtectionControl = 0xE000ED94;

        constexpr uintptr_t Timer = 0x4001A000;
        constexpr uintptr_t TimerStart = Timer + 0x000;
        constexpr uintptr_t TimerStop = Timer + 0x004;
        constexpr uintptr_t TimerClear = Timer + 0x00C;
        constexpr uintptr_t TimerCaptureFive = Timer + 0x054;
        constexpr uintptr_t TimerMode = Timer + 0x504;
        constexpr uintptr_t TimerBitMode = Timer + 0x508;
        constexpr uintptr_t TimerPrescaler = Timer + 0x510;
        constexpr uintptr_t TimerCompareFive = Timer + 0x554;

        constexpr uintptr_t FactoryInformation = 0x10000000;
        constexpr uintptr_t DeviceAddressZero = FactoryInformation + 0x0A4;
        constexpr uintptr_t DeviceAddressOne = FactoryInformation + 0x0A8;

        constexpr uint32_t ReadyStartShortcut = 1u << 0;
        constexpr uint32_t EndDisableShortcut = 1u << 1;
        constexpr uint32_t RadioShortcutsValue = ReadyStartShortcut | EndDisableShortcut;

        constexpr uint32_t AdvertisingAccessAddress = 0x8E89BED6;
        constexpr uint32_t AdvertisingCRCInitialization = 0x555555;
        constexpr uint32_t CRCPolynomial = 0x00065B;

        constexpr uint8_t AdvertisingChannels[] = {37, 38, 39};
        constexpr uint32_t AdvertisingInterval = 100000;
        constexpr uint32_t AdvertisingReceiveTimeout = 700;
        constexpr uint32_t RadioRampTime = 40;
        constexpr uint32_t ReceiveLeadTime = 160;
        constexpr uint32_t InterframeSpacing = 150;

        constexpr uint8_t AdvertisingPDU = 0x00;
        constexpr uint8_t ScanRequestPDU = 0x03;
        constexpr uint8_t ScanResponsePDU = 0x04;
        constexpr uint8_t ConnectionRequestPDU = 0x05;
        constexpr uint8_t RandomAddressFlag = 1u << 6;

        constexpr uint8_t LinkLayerContinuation = 0x01;
        constexpr uint8_t LinkLayerStart = 0x02;
        constexpr uint8_t LinkLayerControl = 0x03;

        constexpr uint16_t ATTChannel = 0x0004;
        constexpr uint16_t SecurityChannel = 0x0006;

        constexpr uint8_t LinkLayerEncryptionRequest = 0x03;
        constexpr uint8_t LinkLayerEncryptionResponse = 0x04;
        constexpr uint8_t LinkLayerStartEncryptionRequest = 0x05;
        constexpr uint8_t LinkLayerStartEncryptionResponse = 0x06;
        constexpr uint8_t LinkLayerReject = 0x0D;

        constexpr uint8_t SecurityPairingRequest = 0x01;
        constexpr uint8_t SecurityPairingResponse = 0x02;
        constexpr uint8_t SecurityPairingConfirm = 0x03;
        constexpr uint8_t SecurityPairingRandom = 0x04;
        constexpr uint8_t SecurityPairingFailed = 0x05;
        constexpr uint8_t SecurityEncryptionInformation = 0x06;
        constexpr uint8_t SecurityCentralIdentification = 0x07;
        constexpr uint8_t SecurityRequest = 0x0B;

        constexpr uint8_t SecurityAuthenticationBonding = 0x01;
        constexpr uint8_t SecurityEncryptionKey = 0x01;
        constexpr uint8_t SecurityNoInputNoOutput = 0x03;
        constexpr uint8_t SecurityMaximumKeySize = 16;

        constexpr uint8_t ATTErrorResponse = 0x01;
        constexpr uint8_t ATTExchangeMTURequest = 0x02;
        constexpr uint8_t ATTExchangeMTUResponse = 0x03;
        constexpr uint8_t ATTFindInformationRequest = 0x04;
        constexpr uint8_t ATTFindInformationResponse = 0x05;
        constexpr uint8_t ATTFindByTypeValueRequest = 0x06;
        constexpr uint8_t ATTFindByTypeValueResponse = 0x07;
        constexpr uint8_t ATTReadByTypeRequest = 0x08;
        constexpr uint8_t ATTReadByTypeResponse = 0x09;
        constexpr uint8_t ATTReadRequest = 0x0A;
        constexpr uint8_t ATTReadResponse = 0x0B;
        constexpr uint8_t ATTReadBlobRequest = 0x0C;
        constexpr uint8_t ATTReadBlobResponse = 0x0D;
        constexpr uint8_t ATTReadByGroupTypeRequest = 0x10;
        constexpr uint8_t ATTReadByGroupTypeResponse = 0x11;
        constexpr uint8_t ATTWriteRequest = 0x12;
        constexpr uint8_t ATTWriteResponse = 0x13;
        constexpr uint8_t ATTHandleValueNotification = 0x1B;
        constexpr uint8_t ATTWriteCommand = 0x52;

        constexpr uint8_t ATTInvalidHandle = 0x01;
        constexpr uint8_t ATTReadNotPermitted = 0x02;
        constexpr uint8_t ATTWriteNotPermitted = 0x03;
        constexpr uint8_t ATTInsufficientAuthentication = 0x05;
        constexpr uint8_t ATTRequestNotSupported = 0x06;
        constexpr uint8_t ATTInvalidOffset = 0x07;
        constexpr uint8_t ATTAttributeNotFound = 0x0A;

        constexpr uint16_t PrimaryServiceUUID = 0x2800;
        constexpr uint16_t CharacteristicUUID = 0x2803;
        constexpr uint16_t ClientCharacteristicConfigurationUUID = 0x2902;
        constexpr uint16_t ReportReferenceUUID = 0x2908;
        constexpr uint16_t GenericAccessUUID = 0x1800;
        constexpr uint16_t GenericAttributeUUID = 0x1801;
        constexpr uint16_t DeviceInformationUUID = 0x180A;
        constexpr uint16_t BatteryServiceUUID = 0x180F;
        constexpr uint16_t HIDServiceUUID = 0x1812;
        constexpr uint16_t DeviceNameUUID = 0x2A00;
        constexpr uint16_t AppearanceUUID = 0x2A01;
        constexpr uint16_t ServiceChangedUUID = 0x2A05;
        constexpr uint16_t BatteryLevelUUID = 0x2A19;
        constexpr uint16_t ManufacturerNameUUID = 0x2A29;
        constexpr uint16_t HIDInformationUUID = 0x2A4A;
        constexpr uint16_t ReportMapUUID = 0x2A4B;
        constexpr uint16_t HIDControlPointUUID = 0x2A4C;
        constexpr uint16_t ReportUUID = 0x2A4D;
        constexpr uint16_t ProtocolModeUUID = 0x2A4E;
        constexpr uint16_t PnPIDUUID = 0x2A50;

        constexpr uint16_t DeviceInformationServiceHandle = 9;
        constexpr uint16_t PnPIDHandle = 13;
        constexpr uint16_t HIDServiceHandle = 14;
        constexpr uint16_t HIDInformationHandle = 16;
        constexpr uint16_t ReportMapHandle = 18;
        constexpr uint16_t HIDControlPointHandle = 20;
        constexpr uint16_t ProtocolModeHandle = 22;
        constexpr uint16_t KeyboardReportHandle = 24;
        constexpr uint16_t KeyboardReportReferenceHandle = 25;
        constexpr uint16_t KeyboardConfigurationHandle = 26;
        constexpr uint16_t BatteryServiceHandle = 27;
        constexpr uint16_t BatteryLevelHandle = 29;
        constexpr uint16_t BatteryConfigurationHandle = 30;
        constexpr uint16_t LastAttributeHandle = BatteryConfigurationHandle;

        constexpr uint16_t KeyboardAppearance = 0x03C1;

        constexpr uint8_t KeyboardReportMap[] = {

            0x05, 0x01,
            0x09, 0x06,
            0xA1, 0x01,
            0x05, 0x07,
            0x19, 0xE0,
            0x29, 0xE7,
            0x15, 0x00,
            0x25, 0x01,
            0x75, 0x01,
            0x95, 0x08,
            0x81, 0x02,
            0x95, 0x01,
            0x75, 0x08,
            0x81, 0x01,
            0x95, 0x06,
            0x75, 0x08,
            0x15, 0x00,
            0x25, 0x65,
            0x05, 0x07,
            0x19, 0x00,
            0x29, 0x65,
            0x81, 0x00,
            0xC0

        };

        struct ServiceDefinition {

            uint16_t Start;
            uint16_t End;
            uint16_t UUID;

        };

        constexpr ServiceDefinition Services[] = {

            {1, 5, GenericAccessUUID},
            {6, 8, GenericAttributeUUID},
            {9, 13, DeviceInformationUUID},
            {14, 26, HIDServiceUUID},
            {27, 30, BatteryServiceUUID}

        };

        struct CharacteristicDefinition {

            uint16_t Declaration;
            uint8_t Properties;
            uint16_t Value;
            uint16_t UUID;

        };

        constexpr CharacteristicDefinition Characteristics[] = {

            {2, 0x02, 3, DeviceNameUUID},
            {4, 0x02, 5, AppearanceUUID},
            {7, 0x20, 8, ServiceChangedUUID},
            {10, 0x02, 11, ManufacturerNameUUID},
            {12, 0x02, 13, PnPIDUUID},
            {15, 0x02, 16, HIDInformationUUID},
            {17, 0x02, 18, ReportMapUUID},
            {19, 0x04, 20, HIDControlPointUUID},
            {21, 0x06, 22, ProtocolModeUUID},
            {23, 0x12, 24, ReportUUID},
            {28, 0x12, 29, BatteryLevelUUID}

        };

        constexpr uint16_t AttributeTypes[] = {

            PrimaryServiceUUID,
            CharacteristicUUID,
            DeviceNameUUID,
            CharacteristicUUID,
            AppearanceUUID,
            PrimaryServiceUUID,
            CharacteristicUUID,
            ServiceChangedUUID,
            PrimaryServiceUUID,
            CharacteristicUUID,
            ManufacturerNameUUID,
            CharacteristicUUID,
            PnPIDUUID,
            PrimaryServiceUUID,
            CharacteristicUUID,
            HIDInformationUUID,
            CharacteristicUUID,
            ReportMapUUID,
            CharacteristicUUID,
            HIDControlPointUUID,
            CharacteristicUUID,
            ProtocolModeUUID,
            CharacteristicUUID,
            ReportUUID,
            ReportReferenceUUID,
            ClientCharacteristicConfigurationUUID,
            PrimaryServiceUUID,
            CharacteristicUUID,
            BatteryLevelUUID,
            ClientCharacteristicConfigurationUUID

        };

        constexpr uint8_t ATTMaximumTransmissionUnit = 23;

        constexpr uintptr_t BondPage = 0x0007F000;
        constexpr uint32_t BondMagic = 0x4D574254;
        constexpr uint32_t BondVersion = 1;

        uint32_t DisableInterrupts() {

            uint32_t State = 0;
            __asm__ volatile("mrs %0, primask" : "=r"(State));
            __asm__ volatile("cpsid i" ::: "memory");
            return State;

        };

        void RestoreInterrupts(uint32_t State) {

            if(State == 0) {

                __asm__ volatile("cpsie i" ::: "memory");

            };

        };

        bool TimeReached(uint32_t Current, uint32_t Target) {

            return static_cast<int32_t>(Current - Target) >= 0;

        };

        uint8_t GetRadioFrequency(uint8_t Channel) {

            if(Channel == 37) {

                return 2;

            };

            if(Channel == 38) {

                return 26;

            };

            if(Channel == 39) {

                return 80;

            };

            if(Channel <= 10) {

                return static_cast<uint8_t>(4 + (Channel * 2));

            };

            return static_cast<uint8_t>(28 + ((Channel - 11) * 2));

        };

    };

    void BluetoothDriver::OnStart() {

        DriverWatchdogScope Watchdog(*this);
        this->StartTimer();
        if(!this->StartHighFrequencyClock()) {

            this->StopTimer();
            this->State = LinkState::Stopped;
            return;

        };

        this->CreateAddress();
        this->LoadBond();
        this->CreateAdvertisement();
        this->CreateScanResponse();
        this->ConfigureCommonRadio();

        this->AdvertisingChannelIndex = 0;
        this->NextAdvertisement = this->GetMicroseconds();
        this->State = LinkState::Advertising;

    };

    void BluetoothDriver::OnStop() {

        DriverWatchdogScope Watchdog(*this);
        this->DisableRadio();
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACEnable) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioPower) = 0;
        this->StopTimer();
        this->State = LinkState::Stopped;

    };

    void BluetoothDriver::OnPanic() {

        *reinterpret_cast<volatile uint32_t*>(RadioShortcuts) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioInterruptClear) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(RadioDisable) = 1;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACEnable) = 0;
        *reinterpret_cast<volatile uint32_t*>(TimerStop) = 1;
        this->State = LinkState::Stopped;

    };

    void BluetoothDriver::Update() {

        DriverWatchdogScope Watchdog(*this);

        if(this->State == LinkState::Advertising) {

            if(TimeReached(this->GetMicroseconds(), this->NextAdvertisement)) {

                this->Advertise();

            };

        } else if(this->State == LinkState::Connecting || this->State == LinkState::Connected) {

            this->UpdateConnection();

        };

    };

    void BluetoothDriver::SetKeyboardReport(uint8_t Modifier, uint8_t KeyCode) {

        if(this->KeyboardReport[0] == Modifier && this->KeyboardReport[2] == KeyCode) {

            return;

        };

        for(uint8_t Index = 0; Index < sizeof(this->KeyboardReport); Index++) {

            this->KeyboardReport[Index] = 0;

        };

        this->KeyboardReport[0] = Modifier;
        this->KeyboardReport[2] = KeyCode;
        this->KeyboardReportDirty = true;

    };

    bool BluetoothDriver::IsAdvertising() const {

        return this->State == LinkState::Advertising;

    };

    bool BluetoothDriver::IsConnected() const {

        return this->State == LinkState::Connected;

    };

    const uint8_t* BluetoothDriver::GetAddress() const {

        return this->Address;

    };

    void BluetoothDriver::StartTimer() {

        *reinterpret_cast<volatile uint32_t*>(TimerStop) = 1;
        *reinterpret_cast<volatile uint32_t*>(TimerMode) = 0;
        *reinterpret_cast<volatile uint32_t*>(TimerBitMode) = 3;
        *reinterpret_cast<volatile uint32_t*>(TimerPrescaler) = 4;
        *reinterpret_cast<volatile uint32_t*>(TimerClear) = 1;
        *reinterpret_cast<volatile uint32_t*>(TimerStart) = 1;

    };

    void BluetoothDriver::StopTimer() {

        *reinterpret_cast<volatile uint32_t*>(TimerStop) = 1;
        *reinterpret_cast<volatile uint32_t*>(TimerClear) = 1;

    };

    uint32_t BluetoothDriver::GetMicroseconds() const {

        *reinterpret_cast<volatile uint32_t*>(TimerCaptureFive) = 1;
        return *reinterpret_cast<volatile uint32_t*>(TimerCompareFive);

    };

    bool BluetoothDriver::StartHighFrequencyClock() {

        *reinterpret_cast<volatile uint32_t*>(ClockHighFrequencyStarted) = 0;
        *reinterpret_cast<volatile uint32_t*>(ClockStartHighFrequency) = 1;

        uint32_t StartedWaiting = this->GetMicroseconds();
        while(*reinterpret_cast<volatile uint32_t*>(ClockHighFrequencyStarted) == 0) {

            if(this->GetMicroseconds() - StartedWaiting >= 100000) {

                return false;

            };

        };

        return true;

    };

    void BluetoothDriver::ConfigureCommonRadio() {

        *reinterpret_cast<volatile uint32_t*>(RadioPower) = 1;
        *reinterpret_cast<volatile uint32_t*>(RadioTransmitPower) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioMode) = 3;
        *reinterpret_cast<volatile uint32_t*>(RadioModeConfiguration) = 1;
        *reinterpret_cast<volatile uint32_t*>(RadioPacketConfigurationZero) = 8 | (1u << 8) | (1u << 20);
        *reinterpret_cast<volatile uint32_t*>(RadioPacketConfigurationOne) = 255 | (3u << 16) | (1u << 25);
        *reinterpret_cast<volatile uint32_t*>(RadioTransmitAddress) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioReceiveAddresses) = 1;
        *reinterpret_cast<volatile uint32_t*>(RadioCRCConfiguration) = 3 | (1u << 8);
        *reinterpret_cast<volatile uint32_t*>(RadioCRCPolynomial) = CRCPolynomial;
        *reinterpret_cast<volatile uint32_t*>(RadioInterframeSpacing) = InterframeSpacing;
        *reinterpret_cast<volatile uint32_t*>(RadioInterruptClear) = 0xFFFFFFFF;

    };

    void BluetoothDriver::ConfigureAdvertisingRadio(uint8_t Channel) {

        *reinterpret_cast<volatile uint32_t*>(RadioBaseZero) = (AdvertisingAccessAddress << 8) & 0xFFFFFF00;
        *reinterpret_cast<volatile uint32_t*>(RadioPrefixZero) = AdvertisingAccessAddress >> 24;
        *reinterpret_cast<volatile uint32_t*>(RadioCRCInitialization) = AdvertisingCRCInitialization;
        *reinterpret_cast<volatile uint32_t*>(RadioFrequency) = GetRadioFrequency(Channel);
        *reinterpret_cast<volatile uint32_t*>(RadioDataWhiteInitialization) = Channel;

    };

    void BluetoothDriver::ConfigureConnectionRadio(uint8_t Channel) {

        *reinterpret_cast<volatile uint32_t*>(RadioBaseZero) = (this->AccessAddress << 8) & 0xFFFFFF00;
        *reinterpret_cast<volatile uint32_t*>(RadioPrefixZero) = this->AccessAddress >> 24;
        *reinterpret_cast<volatile uint32_t*>(RadioCRCInitialization) = this->CRCInitialization;
        *reinterpret_cast<volatile uint32_t*>(RadioFrequency) = GetRadioFrequency(Channel);
        *reinterpret_cast<volatile uint32_t*>(RadioDataWhiteInitialization) = Channel;

    };

    void BluetoothDriver::DisableRadio() {

        if(*reinterpret_cast<volatile uint32_t*>(RadioState) == 0) {

            return;

        };

        *reinterpret_cast<volatile uint32_t*>(RadioDisabled) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioDisable) = 1;

        uint32_t StartedWaiting = this->GetMicroseconds();
        while(*reinterpret_cast<volatile uint32_t*>(RadioDisabled) == 0) {

            if(this->GetMicroseconds() - StartedWaiting >= 1000) {

                break;

            };

        };

    };

    bool BluetoothDriver::TransmitPacket(RadioPacket& Packet, uint32_t AirTime) {

        this->DisableRadio();

        *reinterpret_cast<volatile uint32_t*>(RadioReady) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioEnd) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioDisabled) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioPacketPointer) = reinterpret_cast<uintptr_t>(&Packet);
        *reinterpret_cast<volatile uint32_t*>(RadioShortcuts) = RadioShortcutsValue;

        if(AirTime != 0) {

            uint32_t EnableTime = AirTime - RadioRampTime;
            while(!TimeReached(this->GetMicroseconds(), EnableTime)) {};

        };

        *reinterpret_cast<volatile uint32_t*>(RadioTransmitEnable) = 1;

        uint32_t StartedWaiting = this->GetMicroseconds();
        while(*reinterpret_cast<volatile uint32_t*>(RadioEnd) == 0) {

            if(this->GetMicroseconds() - StartedWaiting >= 2000) {

                this->DisableRadio();
                return false;

            };

        };

        while(*reinterpret_cast<volatile uint32_t*>(RadioDisabled) == 0) {

        };
        return true;

    };

    bool BluetoothDriver::ReceivePacket(RadioPacket& Packet, uint32_t Timeout, uint32_t& AddressTime, uint32_t& EndTime) {

        this->DisableRadio();

        Packet.Header = 0;
        Packet.Length = 0;
        AddressTime = 0;
        EndTime = 0;

        *reinterpret_cast<volatile uint32_t*>(RadioReady) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioAddress) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioEnd) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioDisabled) = 0;
        *reinterpret_cast<volatile uint32_t*>(RadioPacketPointer) = reinterpret_cast<uintptr_t>(&Packet);
        *reinterpret_cast<volatile uint32_t*>(RadioShortcuts) = RadioShortcutsValue;
        *reinterpret_cast<volatile uint32_t*>(RadioReceiveEnable) = 1;

        uint32_t StartedWaiting = this->GetMicroseconds();
        while(*reinterpret_cast<volatile uint32_t*>(RadioEnd) == 0) {

            if(AddressTime == 0 && *reinterpret_cast<volatile uint32_t*>(RadioAddress) != 0) {

                AddressTime = this->GetMicroseconds();

            };

            if(this->GetMicroseconds() - StartedWaiting >= Timeout) {

                this->DisableRadio();
                return false;

            };

        };

        EndTime = this->GetMicroseconds();
        if(AddressTime == 0) {

            AddressTime = EndTime;

        };

        bool Valid = *reinterpret_cast<volatile uint32_t*>(RadioCRCStatus) != 0;
        while(*reinterpret_cast<volatile uint32_t*>(RadioDisabled) == 0) {

        };
        return Valid;

    };

    bool BluetoothDriver::FillRandom(uint8_t* Data, size_t Length) {

        if(Data == nullptr) {

            return false;

        };

        *reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorConfiguration) = 1;
        *reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorValueReady) = 0;
        *reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorStart) = 1;

        for(size_t Index = 0; Index < Length; Index++) {

            uint32_t StartedWaiting = this->GetMicroseconds();
            while(*reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorValueReady) == 0) {

                if(this->GetMicroseconds() - StartedWaiting >= 1000) {

                    *reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorStop) = 1;
                    return false;

                };

            };

            Data[Index] = static_cast<uint8_t>(*reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorValue));
            *reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorValueReady) = 0;

        };

        *reinterpret_cast<volatile uint32_t*>(RandomNumberGeneratorStop) = 1;
        return true;

    };

    bool BluetoothDriver::EncryptBlock(const uint8_t* Key, const uint8_t* Cleartext, uint8_t* Ciphertext) {

        if(Key == nullptr || Cleartext == nullptr || Ciphertext == nullptr) {

            return false;

        };

        for(uint8_t Index = 0; Index < 16; Index++) {

            this->EncryptionBlock.Key[Index] = Key[Index];
            this->EncryptionBlock.Cleartext[Index] = Cleartext[Index];
            this->EncryptionBlock.Ciphertext[Index] = 0;

        };

        *reinterpret_cast<volatile uint32_t*>(ElectronicCodebookEnd) = 0;
        *reinterpret_cast<volatile uint32_t*>(ElectronicCodebookError) = 0;
        *reinterpret_cast<volatile uint32_t*>(ElectronicCodebookDataPointer) =
            reinterpret_cast<uintptr_t>(&this->EncryptionBlock);
        *reinterpret_cast<volatile uint32_t*>(ElectronicCodebookStart) = 1;

        uint32_t StartedWaiting = this->GetMicroseconds();
        while(*reinterpret_cast<volatile uint32_t*>(ElectronicCodebookEnd) == 0) {

            if(*reinterpret_cast<volatile uint32_t*>(ElectronicCodebookError) != 0 ||
                this->GetMicroseconds() - StartedWaiting >= 1000) {

                return false;

            };

        };

        for(uint8_t Index = 0; Index < 16; Index++) {

            Ciphertext[Index] = this->EncryptionBlock.Ciphertext[Index];

        };

        return true;

    };

    bool BluetoothDriver::EncryptLinkPacket(const RadioPacket& Input, RadioPacket& Output,
        uint64_t Counter, bool CentralDirection) {

        return this->ProcessCCM(Input, Output, Counter, CentralDirection, false);

    };

    bool BluetoothDriver::DecryptLinkPacket(const RadioPacket& Input, RadioPacket& Output,
        uint64_t Counter, bool CentralDirection) {

        return this->ProcessCCM(Input, Output, Counter, CentralDirection, true);

    };

    bool BluetoothDriver::ProcessCCM(const RadioPacket& Input, RadioPacket& Output,
        uint64_t Counter, bool CentralDirection, bool Decrypt) {

        if(Input.Length == 0) {

            Output = Input;
            return true;

        };

        Output = {};
        for(uint8_t Index = 0; Index < 16; Index++) {

            this->CCMConfiguration.Key[Index] = this->SessionKey[Index];

        };

        for(uint8_t Index = 0; Index < sizeof(this->CCMConfiguration.PacketCounter); Index++) {

            this->CCMConfiguration.PacketCounter[Index] = 0;

        };

        for(uint8_t Index = 0; Index < 5; Index++) {

            this->CCMConfiguration.PacketCounter[Index] = static_cast<uint8_t>(Counter >> (Index * 8));

        };

        this->CCMConfiguration.PacketCounter[4] &= 0x7F;
        this->CCMConfiguration.Direction = CentralDirection ? 1 : 0;
        for(uint8_t Index = 0; Index < sizeof(this->SessionInitializationVector); Index++) {

            this->CCMConfiguration.InitializationVector[Index] = this->SessionInitializationVector[Index];

        };

        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACEnable) = 2;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACMode) = Decrypt ? 1 : 0;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACHeaderMask) = 0xE3;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACConfigurationPointer) =
            reinterpret_cast<uintptr_t>(&this->CCMConfiguration);
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACInputPointer) =
            reinterpret_cast<uintptr_t>(&Input);
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACOutputPointer) =
            reinterpret_cast<uintptr_t>(&Output);
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACScratchPointer) =
            reinterpret_cast<uintptr_t>(this->CCMScratch);
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACEndCrypt) = 0;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACError) = 0;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACShortcuts) = 1;
        *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACGenerate) = 1;

        uint32_t StartedWaiting = this->GetMicroseconds();
        while(*reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACEndCrypt) == 0) {

            if(*reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACError) != 0 ||
                this->GetMicroseconds() - StartedWaiting >= 1000) {

                return false;

            };

        };

        return !Decrypt || *reinterpret_cast<volatile uint32_t*>(CounterWithCBCMACMICStatus) != 0;

    };

    void BluetoothDriver::CreateAddress() {

        uint32_t Low = *reinterpret_cast<volatile uint32_t*>(DeviceAddressZero);
        uint32_t High = *reinterpret_cast<volatile uint32_t*>(DeviceAddressOne);

        for(uint8_t Index = 0; Index < 4; Index++) {

            this->Address[Index] = static_cast<uint8_t>(Low >> (Index * 8));

        };

        this->Address[4] = static_cast<uint8_t>(High);
        this->Address[5] = static_cast<uint8_t>((High >> 8) | 0xC0);

    };

    void BluetoothDriver::CreateAdvertisement() {

        this->AdvertisementPacket = {};
        this->AdvertisementPacket.Header = AdvertisingPDU | RandomAddressFlag;

        uint8_t Index = 0;
        for(uint8_t AddressIndex = 0; AddressIndex < sizeof(this->Address); AddressIndex++) {

            this->AdvertisementPacket.Payload[Index++] = this->Address[AddressIndex];

        };

        this->AdvertisementPacket.Payload[Index++] = 2;
        this->AdvertisementPacket.Payload[Index++] = 0x01;
        this->AdvertisementPacket.Payload[Index++] = 0x06;

        uint8_t NameLength = 0;
        while(DeviceName[NameLength] != '\0') {

            NameLength++;

        };

        this->AdvertisementPacket.Payload[Index++] = static_cast<uint8_t>(NameLength + 1);
        this->AdvertisementPacket.Payload[Index++] = 0x09;
        for(uint8_t NameIndex = 0; NameIndex < NameLength; NameIndex++) {

            this->AdvertisementPacket.Payload[Index++] = static_cast<uint8_t>(DeviceName[NameIndex]);

        };

        this->AdvertisementPacket.Payload[Index++] = 3;
        this->AdvertisementPacket.Payload[Index++] = 0x03;
        this->AdvertisementPacket.Payload[Index++] = static_cast<uint8_t>(HIDServiceUUID);
        this->AdvertisementPacket.Payload[Index++] = static_cast<uint8_t>(HIDServiceUUID >> 8);

        this->AdvertisementPacket.Payload[Index++] = 3;
        this->AdvertisementPacket.Payload[Index++] = 0x19;
        this->AdvertisementPacket.Payload[Index++] = static_cast<uint8_t>(KeyboardAppearance);
        this->AdvertisementPacket.Payload[Index++] = static_cast<uint8_t>(KeyboardAppearance >> 8);

        this->AdvertisementPacket.Length = Index;

    };

    void BluetoothDriver::CreateScanResponse() {

        this->ScanResponsePacket = {};
        this->ScanResponsePacket.Header = ScanResponsePDU | RandomAddressFlag;

        uint8_t Index = 0;
        for(uint8_t AddressIndex = 0; AddressIndex < sizeof(this->Address); AddressIndex++) {

            this->ScanResponsePacket.Payload[Index++] = this->Address[AddressIndex];

        };

        uint8_t NameLength = 0;
        while(DeviceName[NameLength] != '\0') {

            NameLength++;

        };

        this->ScanResponsePacket.Payload[Index++] = static_cast<uint8_t>(NameLength + 1);
        this->ScanResponsePacket.Payload[Index++] = 0x09;
        for(uint8_t NameIndex = 0; NameIndex < NameLength; NameIndex++) {

            this->ScanResponsePacket.Payload[Index++] = static_cast<uint8_t>(DeviceName[NameIndex]);

        };

        this->ScanResponsePacket.Length = Index;

    };

    void BluetoothDriver::Advertise() {

        uint8_t Channel = AdvertisingChannels[this->AdvertisingChannelIndex];
        this->AdvertisingChannelIndex++;
        if(this->AdvertisingChannelIndex >= sizeof(AdvertisingChannels)) {

            this->AdvertisingChannelIndex = 0;
            this->NextAdvertisement += AdvertisingInterval;

        } else {

            this->NextAdvertisement = this->GetMicroseconds() + 500;

        };

        this->ConfigureAdvertisingRadio(Channel);
        if(!this->TransmitPacket(this->AdvertisementPacket)) {

            return;

        };

        uint32_t AddressTime = 0;
        uint32_t EndTime = 0;
        if(this->ReceivePacket(this->ReceiveBuffer, AdvertisingReceiveTimeout, AddressTime, EndTime)) {

            this->HandleAdvertisingPacket(EndTime);

        };

    };

    void BluetoothDriver::HandleAdvertisingPacket(uint32_t EndTime) {

        uint8_t Type = this->ReceiveBuffer.Header & 0x0F;
        if(this->ReceiveBuffer.Length < 12) {

            return;

        };

        for(uint8_t Index = 0; Index < sizeof(this->Address); Index++) {

            if(this->ReceiveBuffer.Payload[Index + 6] != this->Address[Index]) {

                return;

            };

        };

        if(Type == ScanRequestPDU && this->ReceiveBuffer.Length == 12) {

            this->TransmitPacket(this->ScanResponsePacket, EndTime + InterframeSpacing);

        } else if(Type == ConnectionRequestPDU && this->ReceiveBuffer.Length == 34) {

            this->BeginConnection(EndTime);

        };

    };

    bool BluetoothDriver::BeginConnection(uint32_t EndTime) {

        for(uint8_t Index = 0; Index < sizeof(this->PeerAddress); Index++) {

            this->PeerAddress[Index] = this->ReceiveBuffer.Payload[Index];

        };

        this->PeerAddressType = (this->ReceiveBuffer.Header >> 6) & 1;
        const uint8_t* LinkData = &this->ReceiveBuffer.Payload[12];

        uint32_t AccessAddress = Read32(&LinkData[0]);
        uint32_t CRCInitialization = Read24(&LinkData[4]);
        uint8_t WindowSize = LinkData[7];
        uint16_t WindowOffset = Read16(&LinkData[8]);
        uint16_t ConnectionInterval = Read16(&LinkData[10]);
        uint16_t ConnectionLatency = Read16(&LinkData[12]);
        uint16_t SupervisionTimeout = Read16(&LinkData[14]);
        uint8_t HopIncrement = LinkData[21] & 0x1F;

        if(AccessAddress == AdvertisingAccessAddress || WindowSize == 0 || ConnectionInterval < 6 ||
            ConnectionInterval > 3200 || ConnectionLatency > 499 || SupervisionTimeout < 10 ||
            SupervisionTimeout > 3200 || HopIncrement < 5 || HopIncrement > 16) {

            return false;

        };

        this->AccessAddress = AccessAddress;
        this->CRCInitialization = CRCInitialization;
        this->WindowSize = WindowSize;
        this->WindowOffset = WindowOffset;
        this->ConnectionInterval = ConnectionInterval;
        this->ConnectionLatency = ConnectionLatency;
        this->SupervisionTimeout = SupervisionTimeout;
        this->HopIncrement = HopIncrement;

        for(uint8_t Index = 0; Index < ChannelMapSize; Index++) {

            this->ChannelMap[Index] = LinkData[16 + Index];

        };

        this->ChannelMap[4] &= 0x1F;
        if(this->GetUsedDataChannelCount() < 2) {

            return false;

        };

        this->LastUnmappedChannel = 0;
        this->FirstWindowStart = EndTime + 1250 + (static_cast<uint32_t>(WindowOffset) * 1250);
        this->FirstWindowEnd = this->FirstWindowStart + (static_cast<uint32_t>(WindowSize) * 1250);
        this->NextConnectionAnchor = 0;
        this->LastPacketTime = EndTime;
        this->ConnectionEventCounter = 0;
        this->ExpectedReceiveSequence = 0;
        this->TransmitSequence = 0;
        this->TransmitOutstanding = false;
        this->PendingLength = 0;
        this->PendingLinkLayerID = LinkLayerContinuation;
        this->ConnectionUpdatePending = false;
        this->ChannelMapUpdatePending = false;
        this->ProtocolMode = 1;
        this->KeyboardNotificationsEnabled = false;
        this->BatteryNotificationsEnabled = false;
        this->KeyboardReportDirty = true;
        this->Pairing = this->BondValid ? PairingState::Bonded : PairingState::Idle;
        this->Encryption = EncryptionState::Disabled;
        this->SecurityRequested = false;
        this->ReceiveEncryptionEnabled = false;
        this->TransmitEncryptionEnabled = false;
        this->UsingBondKey = false;
        this->ReceivePacketCounter = 0;
        this->TransmitPacketCounter = 0;
        this->OutstandingTransmitPacketCounter = 0;
        this->State = LinkState::Connecting;
        return true;

    };

    void BluetoothDriver::UpdateConnection() {

        uint32_t Current = this->GetMicroseconds();

        if(this->State == LinkState::Connecting) {

            uint32_t ReceiveStart = this->FirstWindowStart - ReceiveLeadTime;
            if(!TimeReached(Current, ReceiveStart)) {

                return;

            };

            uint8_t Channel = this->GetNextDataChannel();
            uint32_t Timeout = (this->FirstWindowEnd - ReceiveStart) + 400;
            if(!this->HandleConnectionEvent(Channel, Timeout, true)) {

                this->EndConnection();

            };

            return;

        };

        uint32_t SupervisionMicroseconds = static_cast<uint32_t>(this->SupervisionTimeout) * 10000;
        if(Current - this->LastPacketTime >= SupervisionMicroseconds) {

            this->EndConnection();
            return;

        };

        uint32_t IntervalMicroseconds = static_cast<uint32_t>(this->ConnectionInterval) * 1250;
        while(TimeReached(Current, this->NextConnectionAnchor + IntervalMicroseconds)) {

            this->NextConnectionAnchor += IntervalMicroseconds;
            this->ConnectionEventCounter++;
            (void)this->GetNextDataChannel();

        };

        if(!TimeReached(Current, this->NextConnectionAnchor - ReceiveLeadTime)) {

            return;

        };

        if(this->ConnectionUpdatePending && this->ConnectionEventCounter == this->ConnectionUpdateInstant) {

            this->WindowSize = this->PendingWindowSize;
            this->WindowOffset = this->PendingWindowOffset;
            this->ConnectionInterval = this->PendingConnectionInterval;
            this->ConnectionLatency = this->PendingConnectionLatency;
            this->SupervisionTimeout = this->PendingSupervisionTimeout;
            this->ConnectionUpdatePending = false;

        };

        if(this->ChannelMapUpdatePending && this->ConnectionEventCounter == this->ChannelMapUpdateInstant) {

            for(uint8_t Index = 0; Index < ChannelMapSize; Index++) {

                this->ChannelMap[Index] = this->PendingChannelMap[Index];

            };

            this->ChannelMapUpdatePending = false;

        };

        uint8_t Channel = this->GetNextDataChannel();
        this->HandleConnectionEvent(Channel, 900, false);
        this->NextConnectionAnchor += static_cast<uint32_t>(this->ConnectionInterval) * 1250;
        this->ConnectionEventCounter++;

    };

    bool BluetoothDriver::HandleConnectionEvent(uint8_t Channel, uint32_t Timeout, bool FirstEvent) {

        uint32_t InterruptState = DisableInterrupts();
        this->ConfigureConnectionRadio(Channel);

        uint32_t AddressTime = 0;
        uint32_t EndTime = 0;
        bool Received = this->ReceivePacket(this->ReceiveBuffer, Timeout, AddressTime, EndTime);
        if(!Received) {

            RestoreInterrupts(InterruptState);
            return false;

        };

        uint8_t EncryptedLength = this->ReceiveBuffer.Length;
        uint8_t CentralNextExpectedSequence = (this->ReceiveBuffer.Header >> 2) & 1;
        uint8_t CentralSequence = (this->ReceiveBuffer.Header >> 3) & 1;
        bool NewReceivePacket = CentralSequence == this->ExpectedReceiveSequence;

        if((this->ReceiveEncryptionEnabled && EncryptedLength > 31) ||
            (!this->ReceiveEncryptionEnabled && EncryptedLength > 27)) {

            RestoreInterrupts(InterruptState);
            return false;

        };

        if(this->ReceiveEncryptionEnabled && EncryptedLength != 0) {

            uint64_t Counter = NewReceivePacket ? this->ReceivePacketCounter :
                (this->ReceivePacketCounter == 0 ? 0 : this->ReceivePacketCounter - 1);

            if(!this->DecryptLinkPacket(this->ReceiveBuffer, this->DecryptedReceiveBuffer,
                Counter, true)) {

                this->EndConnection();
                RestoreInterrupts(InterruptState);
                return false;

            };

            this->ReceiveBuffer = this->DecryptedReceiveBuffer;
            if(NewReceivePacket) {

                this->ReceivePacketCounter++;

            };

        };

        if(this->TransmitOutstanding && CentralNextExpectedSequence != this->TransmitSequence) {

            this->TransmitSequence ^= 1;
            this->TransmitOutstanding = false;
            this->PendingLength = 0;
            this->OnTransmitAcknowledged();

        };

        if(NewReceivePacket) {

            this->ExpectedReceiveSequence ^= 1;
            this->HandleDataPacket();

        };

        if(!this->SecurityRequested && !this->ReceiveEncryptionEnabled && this->PendingLength == 0) {

            this->QueueSecurityRequest();

        };

        if(!this->TransmitOutstanding && this->PendingLength == 0 &&
            this->Encryption == EncryptionState::Enabled &&
            this->KeyboardNotificationsEnabled && this->KeyboardReportDirty) {

            this->QueueKeyboardReport();

        };

        if(this->State == LinkState::Advertising) {

            RestoreInterrupts(InterruptState);
            return true;

        };

        bool NewTransmitPacket = !this->TransmitOutstanding;
        this->CreateConnectionResponse();

        RadioPacket* Packet = &this->TransmitBuffer;
        if(this->TransmitEncryptionEnabled && this->TransmitBuffer.Length != 0) {

            if(NewTransmitPacket) {

                this->OutstandingTransmitPacketCounter = this->TransmitPacketCounter++;

            };

            if(!this->EncryptLinkPacket(this->TransmitBuffer, this->EncryptedTransmitBuffer,
                this->OutstandingTransmitPacketCounter, false)) {

                this->EndConnection();
                RestoreInterrupts(InterruptState);
                return false;

            };

            Packet = &this->EncryptedTransmitBuffer;

        };

        if(!this->TransmitPacket(*Packet, EndTime + InterframeSpacing)) {

            this->EndConnection();
            RestoreInterrupts(InterruptState);
            return false;

        };

        if(NewTransmitPacket && this->PendingLinkLayerID == LinkLayerControl &&
            this->PendingLength != 0 && this->PendingData[0] == LinkLayerStartEncryptionRequest) {

            this->ReceiveEncryptionEnabled = true;

        };

        this->TransmitOutstanding = true;
        this->LastPacketTime = AddressTime - 40;

        if(FirstEvent) {

            this->State = LinkState::Connected;
            this->NextConnectionAnchor = this->LastPacketTime + (static_cast<uint32_t>(this->ConnectionInterval) * 1250);
            this->ConnectionEventCounter = 1;

        };

        RestoreInterrupts(InterruptState);
        return true;

    };

    void BluetoothDriver::EndConnection() {

        this->DisableRadio();
        this->State = LinkState::Advertising;
        this->AdvertisingChannelIndex = 0;
        this->NextAdvertisement = this->GetMicroseconds() + 10000;
        this->PendingLength = 0;
        this->TransmitOutstanding = false;
        this->ReceiveEncryptionEnabled = false;
        this->TransmitEncryptionEnabled = false;
        this->Encryption = EncryptionState::Disabled;

    };

    uint8_t BluetoothDriver::GetNextDataChannel() {

        this->LastUnmappedChannel = static_cast<uint8_t>((this->LastUnmappedChannel + this->HopIncrement) % 37);
        return this->RemapDataChannel(this->LastUnmappedChannel);

    };

    uint8_t BluetoothDriver::RemapDataChannel(uint8_t Channel) const {

        if(this->IsDataChannelEnabled(Channel)) {

            return Channel;

        };

        uint8_t UsedCount = this->GetUsedDataChannelCount();
        uint8_t RemapIndex = Channel % UsedCount;
        for(uint8_t Candidate = 0; Candidate < 37; Candidate++) {

            if(!this->IsDataChannelEnabled(Candidate)) {

                continue;

            };

            if(RemapIndex == 0) {

                return Candidate;

            };

            RemapIndex--;

        };

        return 0;

    };

    bool BluetoothDriver::IsDataChannelEnabled(uint8_t Channel) const {

        if(Channel >= 37) {

            return false;

        };

        return (this->ChannelMap[Channel / 8] & (1u << (Channel % 8))) != 0;

    };

    uint8_t BluetoothDriver::GetUsedDataChannelCount() const {

        uint8_t Count = 0;
        for(uint8_t Channel = 0; Channel < 37; Channel++) {

            if(this->IsDataChannelEnabled(Channel)) {

                Count++;

            };

        };

        return Count;

    };

    void BluetoothDriver::HandleDataPacket() {

        uint8_t LinkLayerID = this->ReceiveBuffer.Header & 0x03;
        if(LinkLayerID == LinkLayerControl) {

            this->HandleControlPacket();

        } else if(LinkLayerID == LinkLayerStart) {

            this->HandleL2CAPPacket();

        };

    };

    void BluetoothDriver::HandleControlPacket() {

        if(this->ReceiveBuffer.Length == 0) {

            return;

        };

        const uint8_t* Request = this->ReceiveBuffer.Payload;
        alignas(4) uint8_t Response[16] = {};

        switch(Request[0]) {

            case 0x00:

                if(this->ReceiveBuffer.Length >= 12) {

                    this->PendingWindowSize = Request[1];
                    this->PendingWindowOffset = Read16(&Request[2]);
                    this->PendingConnectionInterval = Read16(&Request[4]);
                    this->PendingConnectionLatency = Read16(&Request[6]);
                    this->PendingSupervisionTimeout = Read16(&Request[8]);
                    this->ConnectionUpdateInstant = Read16(&Request[10]);
                    this->ConnectionUpdatePending = true;

                };

                return;

            case 0x01:

                if(this->ReceiveBuffer.Length >= 8) {

                    for(uint8_t Index = 0; Index < ChannelMapSize; Index++) {

                        this->PendingChannelMap[Index] = Request[Index + 1];

                    };

                    this->PendingChannelMap[4] &= 0x1F;
                    this->ChannelMapUpdateInstant = Read16(&Request[6]);
                    this->ChannelMapUpdatePending = true;

                };

                return;

            case 0x02:

                this->EndConnection();
                return;

            case LinkLayerEncryptionRequest: {

                if(this->ReceiveBuffer.Length < 23) {

                    return;

                };

                uint16_t Diversifier = Read16(&Request[9]);
                bool RandomIsZero = true;
                for(uint8_t Index = 0; Index < 8; Index++) {

                    if(Request[Index + 1] != 0) {

                        RandomIsZero = false;

                    };

                };

                const uint8_t* Key = nullptr;
                if(this->Pairing == PairingState::WaitingForEncryption && Diversifier == 0 && RandomIsZero) {

                    Key = this->ShortTermKey;
                    this->UsingBondKey = false;

                } else if(this->BondValid && Diversifier == this->BondDiversifier) {

                    bool RandomMatches = true;
                    for(uint8_t Index = 0; Index < sizeof(this->BondRandom); Index++) {

                        if(Request[Index + 1] != this->BondRandom[Index]) {

                            RandomMatches = false;

                        };

                    };

                    if(RandomMatches) {

                        Key = this->LongTermKey;
                        this->UsingBondKey = true;

                    };

                };

                if(Key == nullptr || !this->FillRandom(&Response[1], 12) ||
                    !this->CreateSessionKey(Key, &Request[11], &Response[1])) {

                    Response[0] = LinkLayerReject;
                    Response[1] = 0x06;
                    this->QueueControlResponse(Response, 2);
                    return;

                };

                for(uint8_t Index = 0; Index < 4; Index++) {

                    this->SessionInitializationVector[Index] = Request[Index + 19];
                    this->SessionInitializationVector[Index + 4] = Response[Index + 9];

                };

                this->ReceivePacketCounter = 0;
                this->TransmitPacketCounter = 0;
                Response[0] = LinkLayerEncryptionResponse;
                this->QueueControlResponse(Response, 13);
                this->Encryption = EncryptionState::ResponseSent;
                return;

            };

            case LinkLayerStartEncryptionResponse:

                if(this->Encryption != EncryptionState::WaitingForStartResponse) {

                    return;

                };

                Response[0] = LinkLayerStartEncryptionResponse;
                this->TransmitEncryptionEnabled = true;
                this->Encryption = EncryptionState::Enabled;
                this->QueueControlResponse(Response, 1);
                return;

            case 0x08:

                Response[0] = 0x09;
                Response[1] = 0x01;
                this->QueueControlResponse(Response, 9);
                return;

            case 0x0C:

                Response[0] = 0x0C;
                Response[1] = 0x08;
                Response[2] = 0xFF;
                Response[3] = 0xFF;
                this->QueueControlResponse(Response, 6);
                return;

            case 0x12:

                Response[0] = 0x13;
                this->QueueControlResponse(Response, 1);
                return;

            case 0x14:

                Response[0] = 0x15;
                Write16(&Response[1], 27);
                Write16(&Response[3], 328);
                Write16(&Response[5], 27);
                Write16(&Response[7], 328);
                this->QueueControlResponse(Response, 9);
                return;

            case 0x16:

                Response[0] = 0x17;
                Response[1] = 1;
                Response[2] = 1;
                this->QueueControlResponse(Response, 3);
                return;

            case 0x18:

                return;

            default:

                Response[0] = 0x07;
                Response[1] = Request[0];
                this->QueueControlResponse(Response, 2);
                return;

        };

    };

    void BluetoothDriver::HandleL2CAPPacket() {

        if(this->ReceiveBuffer.Length < 4) {

            return;

        };

        uint16_t Length = Read16(&this->ReceiveBuffer.Payload[0]);
        uint16_t Channel = Read16(&this->ReceiveBuffer.Payload[2]);
        if(Length > this->ReceiveBuffer.Length - 4) {

            return;

        };

        if(Channel == ATTChannel) {

            this->HandleATTPacket(&this->ReceiveBuffer.Payload[4], static_cast<uint8_t>(Length));

        } else if(Channel == SecurityChannel) {

            this->HandleSecurityPacket(&this->ReceiveBuffer.Payload[4], static_cast<uint8_t>(Length));

        };

    };

    void BluetoothDriver::HandleSecurityPacket(const uint8_t* Request, uint8_t Length) {

        if(Request == nullptr || Length == 0) {

            return;

        };

        alignas(4) uint8_t Response[20] = {};

        if(Request[0] == SecurityPairingRequest && Length == 7) {

            if(Request[4] < 7 || Request[4] > SecurityMaximumKeySize) {

                Response[0] = SecurityPairingFailed;
                Response[1] = 0x06;
                this->QueueSecurityResponse(Response, 2);
                return;

            };

            for(uint8_t Index = 0; Index < sizeof(this->PairingRequest); Index++) {

                this->PairingRequest[Index] = Request[Index];

            };

            this->PairingResponse[0] = SecurityPairingResponse;
            this->PairingResponse[1] = SecurityNoInputNoOutput;
            this->PairingResponse[2] = 0;
            this->PairingResponse[3] = SecurityAuthenticationBonding;
            this->PairingResponse[4] = Request[4];
            this->PairingResponse[5] = 0;
            this->PairingResponse[6] = Request[6] & SecurityEncryptionKey;

            if(this->PairingResponse[6] == 0 ||
                !this->FillRandom(this->PeripheralRandom, sizeof(this->PeripheralRandom)) ||
                !this->FillRandom(this->LongTermKey, sizeof(this->LongTermKey)) ||
                !this->FillRandom(this->BondRandom, sizeof(this->BondRandom)) ||
                !this->FillRandom(reinterpret_cast<uint8_t*>(&this->BondDiversifier),
                    sizeof(this->BondDiversifier))) {

                Response[0] = SecurityPairingFailed;
                Response[1] = 0x05;
                this->QueueSecurityResponse(Response, 2);
                return;

            };

            this->BondValid = false;
            this->UsingBondKey = false;
            this->Pairing = PairingState::WaitingForConfirm;
            this->QueueSecurityResponse(this->PairingResponse, sizeof(this->PairingResponse));
            return;

        };

        if(Request[0] == SecurityPairingConfirm && Length == 17 &&
            this->Pairing == PairingState::WaitingForConfirm) {

            for(uint8_t Index = 0; Index < sizeof(this->CentralConfirm); Index++) {

                this->CentralConfirm[Index] = Request[Index + 1];

            };

            Response[0] = SecurityPairingConfirm;
            if(!this->CreateConfirm(this->PeripheralRandom, &Response[1])) {

                Response[0] = SecurityPairingFailed;
                Response[1] = 0x08;
                this->QueueSecurityResponse(Response, 2);
                return;

            };

            this->Pairing = PairingState::WaitingForRandom;
            this->QueueSecurityResponse(Response, 17);
            return;

        };

        if(Request[0] == SecurityPairingRandom && Length == 17 &&
            this->Pairing == PairingState::WaitingForRandom) {

            for(uint8_t Index = 0; Index < sizeof(this->CentralRandom); Index++) {

                this->CentralRandom[Index] = Request[Index + 1];

            };

            alignas(4) uint8_t ExpectedConfirm[16] = {};
            if(!this->CreateConfirm(this->CentralRandom, ExpectedConfirm)) {

                Response[0] = SecurityPairingFailed;
                Response[1] = 0x08;
                this->QueueSecurityResponse(Response, 2);
                return;

            };

            for(uint8_t Index = 0; Index < sizeof(this->CentralConfirm); Index++) {

                if(ExpectedConfirm[Index] != this->CentralConfirm[Index]) {

                    Response[0] = SecurityPairingFailed;
                    Response[1] = 0x04;
                    this->QueueSecurityResponse(Response, 2);
                    this->Pairing = PairingState::Idle;
                    return;

                };

            };

            if(!this->CreateShortTermKey(this->CentralRandom, this->PeripheralRandom)) {

                Response[0] = SecurityPairingFailed;
                Response[1] = 0x08;
                this->QueueSecurityResponse(Response, 2);
                return;

            };

            Response[0] = SecurityPairingRandom;
            for(uint8_t Index = 0; Index < sizeof(this->PeripheralRandom); Index++) {

                Response[Index + 1] = this->PeripheralRandom[Index];

            };

            this->Pairing = PairingState::WaitingForEncryption;
            this->QueueSecurityResponse(Response, 17);
            return;

        };

        if(Request[0] == SecurityPairingFailed) {

            this->Pairing = PairingState::Idle;
            return;

        };

    };

    void BluetoothDriver::HandleATTPacket(const uint8_t* Request, uint8_t Length) {

        if(Length == 0) {

            return;

        };

        alignas(4) uint8_t Response[ATTMaximumTransmissionUnit + 1] = {};

        if(Request[0] == ATTExchangeMTURequest && Length >= 3) {

            Response[0] = ATTExchangeMTUResponse;
            Write16(&Response[1], ATTMaximumTransmissionUnit);
            this->QueueATTResponse(Response, 3);
            return;

        };

        if(Request[0] == ATTReadByGroupTypeRequest && Length >= 7) {

            uint16_t Start = Read16(&Request[1]);
            uint16_t End = Read16(&Request[3]);
            uint16_t Type = Read16(&Request[5]);
            if(Type != PrimaryServiceUUID) {

                this->QueueATTError(Request[0], Start, ATTAttributeNotFound);
                return;

            };

            uint8_t Index = 2;
            Response[0] = ATTReadByGroupTypeResponse;
            Response[1] = 6;
            for(const ServiceDefinition& Service : Services) {

                if(Service.Start < Start || Service.Start > End || Index + 6 > ATTMaximumTransmissionUnit) {

                    continue;

                };

                Write16(&Response[Index], Service.Start);
                Write16(&Response[Index + 2], Service.End);
                Write16(&Response[Index + 4], Service.UUID);
                Index += 6;

            };

            if(Index == 2) {

                this->QueueATTError(Request[0], Start, ATTAttributeNotFound);

            } else {

                this->QueueATTResponse(Response, Index);

            };

            return;

        };

        if(Request[0] == ATTReadByTypeRequest && Length >= 7) {

            uint16_t Start = Read16(&Request[1]);
            uint16_t End = Read16(&Request[3]);
            uint16_t Type = Read16(&Request[5]);
            uint8_t Index = 2;
            Response[0] = ATTReadByTypeResponse;

            if(Type == CharacteristicUUID) {

                Response[1] = 7;
                for(const CharacteristicDefinition& Characteristic : Characteristics) {

                    if(Characteristic.Declaration < Start || Characteristic.Declaration > End ||
                        Index + 7 > ATTMaximumTransmissionUnit) {

                        continue;

                    };

                    Write16(&Response[Index], Characteristic.Declaration);
                    Response[Index + 2] = Characteristic.Properties;
                    Write16(&Response[Index + 3], Characteristic.Value);
                    Write16(&Response[Index + 5], Characteristic.UUID);
                    Index += 7;

                };

            } else {

                for(uint16_t Handle = Start; Handle <= End && Handle <= LastAttributeHandle; Handle++) {

                    if(AttributeTypes[Handle - 1] != Type) {

                        continue;

                    };

                    uint8_t ValueLength = ATTMaximumTransmissionUnit - 4;
                    if(!this->ReadAttribute(Handle, 0, &Response[4], ValueLength)) {

                        continue;

                    };

                    Response[1] = static_cast<uint8_t>(ValueLength + 2);
                    Write16(&Response[2], Handle);
                    Index = static_cast<uint8_t>(ValueLength + 4);
                    break;

                };

            };

            if(Index == 2) {

                this->QueueATTError(Request[0], Start, ATTAttributeNotFound);

            } else {

                this->QueueATTResponse(Response, Index);

            };

            return;

        };

        if(Request[0] == ATTFindInformationRequest && Length >= 5) {

            uint16_t Start = Read16(&Request[1]);
            uint16_t End = Read16(&Request[3]);
            uint8_t Index = 2;
            Response[0] = ATTFindInformationResponse;
            Response[1] = 1;
            for(uint16_t Handle = Start; Handle <= End && Handle <= LastAttributeHandle && Index <= 18; Handle++) {

                if(Handle == 0) {

                    continue;

                };

                Write16(&Response[Index], Handle);
                Write16(&Response[Index + 2], AttributeTypes[Handle - 1]);
                Index += 4;

            };

            if(Index == 2) {

                this->QueueATTError(Request[0], Start, ATTAttributeNotFound);

            } else {

                this->QueueATTResponse(Response, Index);

            };

            return;

        };

        if(Request[0] == ATTFindByTypeValueRequest && Length >= 9) {

            uint16_t Start = Read16(&Request[1]);
            uint16_t End = Read16(&Request[3]);
            uint16_t Type = Read16(&Request[5]);
            uint16_t Value = Read16(&Request[7]);
            uint8_t Index = 1;
            Response[0] = ATTFindByTypeValueResponse;

            if(Type == PrimaryServiceUUID) {

                for(const ServiceDefinition& Service : Services) {

                    if(Service.UUID != Value || Service.Start < Start || Service.Start > End ||
                        Index + 4 > ATTMaximumTransmissionUnit) {

                        continue;

                    };

                    Write16(&Response[Index], Service.Start);
                    Write16(&Response[Index + 2], Service.End);
                    Index += 4;

                };

            };

            if(Index == 1) {

                this->QueueATTError(Request[0], Start, ATTAttributeNotFound);

            } else {

                this->QueueATTResponse(Response, Index);

            };

            return;

        };

        if((Request[0] == ATTReadRequest && Length >= 3) ||
            (Request[0] == ATTReadBlobRequest && Length >= 5)) {

            uint16_t Handle = Read16(&Request[1]);
            uint16_t Offset = Request[0] == ATTReadBlobRequest ? Read16(&Request[3]) : 0;
            uint8_t ValueLength = ATTMaximumTransmissionUnit - 1;
            Response[0] = Request[0] == ATTReadBlobRequest ? ATTReadBlobResponse : ATTReadResponse;

            if(Handle > HIDServiceHandle && Handle <= KeyboardConfigurationHandle &&
                this->Encryption != EncryptionState::Enabled) {

                this->QueueATTError(Request[0], Handle, ATTInsufficientAuthentication);
                return;

            };

            if(this->ReadAttribute(Handle, Offset, &Response[1], ValueLength)) {

                this->QueueATTResponse(Response, static_cast<uint8_t>(ValueLength + 1));

            } else if(Handle > LastAttributeHandle || Handle == 0) {

                this->QueueATTError(Request[0], Handle, ATTInvalidHandle);

            } else if(Offset != 0) {

                this->QueueATTError(Request[0], Handle, ATTInvalidOffset);

            } else {

                this->QueueATTError(Request[0], Handle, ATTReadNotPermitted);

            };

            return;

        };

        if((Request[0] == ATTWriteRequest && Length >= 3) ||
            (Request[0] == ATTWriteCommand && Length >= 3)) {

            uint16_t Handle = Read16(&Request[1]);

            if(Handle > HIDServiceHandle && Handle <= KeyboardConfigurationHandle &&
                this->Encryption != EncryptionState::Enabled) {

                if(Request[0] == ATTWriteRequest) {

                    this->QueueATTError(Request[0], Handle, ATTInsufficientAuthentication);

                };

                return;

            };

            bool Written = this->WriteAttribute(Handle, &Request[3], static_cast<uint8_t>(Length - 3));
            if(Request[0] == ATTWriteCommand) {

                return;

            };

            if(Written) {

                Response[0] = ATTWriteResponse;
                this->QueueATTResponse(Response, 1);

            } else if(Handle > LastAttributeHandle || Handle == 0) {

                this->QueueATTError(Request[0], Handle, ATTInvalidHandle);

            } else {

                this->QueueATTError(Request[0], Handle, ATTWriteNotPermitted);

            };

            return;

        };

        this->QueueATTError(Request[0], 0, ATTRequestNotSupported);

    };

    bool BluetoothDriver::ReadAttribute(uint16_t Handle, uint16_t Offset, uint8_t* Data, uint8_t& Length) const {

        alignas(4) uint8_t Value[24] = {};
        const uint8_t* Source = Value;
        size_t ValueLength = 0;

        for(const ServiceDefinition& Service : Services) {

            if(Service.Start == Handle) {

                Write16(Value, Service.UUID);
                ValueLength = 2;
                break;

            };

        };

        if(ValueLength == 0) {

            for(const CharacteristicDefinition& Characteristic : Characteristics) {

                if(Characteristic.Declaration == Handle) {

                    Value[0] = Characteristic.Properties;
                    Write16(&Value[1], Characteristic.Value);
                    Write16(&Value[3], Characteristic.UUID);
                    ValueLength = 5;
                    break;

                };

            };

        };

        if(Handle == 3 || Handle == 11) {

            Source = reinterpret_cast<const uint8_t*>(DeviceName);
            while(DeviceName[ValueLength] != '\0') {

                ValueLength++;

            };

        } else if(Handle == 5) {

            Write16(Value, KeyboardAppearance);
            ValueLength = 2;

        } else if(Handle == 8) {

            ValueLength = 4;

        } else if(Handle == PnPIDHandle) {

            Value[0] = 2;
            Write16(&Value[1], 0xFFFF);
            Write16(&Value[3], 1);
            Write16(&Value[5], 1);
            ValueLength = 7;

        } else if(Handle == HIDInformationHandle) {

            Value[0] = 0x11;
            Value[1] = 0x01;
            Value[2] = 0;
            Value[3] = 0x02;
            ValueLength = 4;

        } else if(Handle == ReportMapHandle) {

            Source = KeyboardReportMap;
            ValueLength = sizeof(KeyboardReportMap);

        } else if(Handle == ProtocolModeHandle) {

            Value[0] = this->ProtocolMode;
            ValueLength = 1;

        } else if(Handle == KeyboardReportHandle) {

            Source = this->KeyboardReport;
            ValueLength = sizeof(this->KeyboardReport);

        } else if(Handle == KeyboardReportReferenceHandle) {

            Value[0] = 0;
            Value[1] = 1;
            ValueLength = 2;

        } else if(Handle == KeyboardConfigurationHandle) {

            Write16(Value, this->KeyboardNotificationsEnabled ? 1 : 0);
            ValueLength = 2;

        } else if(Handle == BatteryLevelHandle) {

            Value[0] = 100;
            ValueLength = 1;

        } else if(Handle == BatteryConfigurationHandle) {

            Write16(Value, this->BatteryNotificationsEnabled ? 1 : 0);
            ValueLength = 2;

        };

        if(ValueLength == 0 || Offset > ValueLength) {

            return false;

        };

        size_t Remaining = ValueLength - Offset;
        if(Remaining < Length) {

            Length = static_cast<uint8_t>(Remaining);

        };

        for(uint8_t Index = 0; Index < Length; Index++) {

            Data[Index] = Source[Offset + Index];

        };

        return true;

    };

    bool BluetoothDriver::WriteAttribute(uint16_t Handle, const uint8_t* Data, uint8_t Length) {

        if(Handle == HIDControlPointHandle && Length == 1) {

            return Data[0] <= 1;

        };

        if(Handle == ProtocolModeHandle && Length == 1 && Data[0] <= 1) {

            this->ProtocolMode = Data[0];
            return true;

        };

        if(Handle == KeyboardConfigurationHandle && Length == 2) {

            uint16_t Configuration = Read16(Data);
            if(Configuration > 1) {

                return false;

            };

            this->KeyboardNotificationsEnabled = Configuration == 1;
            this->KeyboardReportDirty = this->KeyboardNotificationsEnabled;
            return true;

        };

        if(Handle == BatteryConfigurationHandle && Length == 2) {

            uint16_t Configuration = Read16(Data);
            if(Configuration > 1) {

                return false;

            };

            this->BatteryNotificationsEnabled = Configuration == 1;
            return true;

        };

        return false;

    };

    bool BluetoothDriver::CreateConfirm(const uint8_t* Random, uint8_t* Confirm) {

        if(Random == nullptr || Confirm == nullptr) {

            return false;

        };

        alignas(4) uint8_t TemporaryKey[16] = {};
        alignas(4) uint8_t FirstValue[16] = {};
        alignas(4) uint8_t SecondValue[16] = {};
        alignas(4) uint8_t Result[16] = {};

        FirstValue[0] = this->PeerAddressType;
        FirstValue[1] = 1;
        for(uint8_t Index = 0; Index < sizeof(this->PairingRequest); Index++) {

            FirstValue[Index + 2] = this->PairingRequest[Index];
            FirstValue[Index + 9] = this->PairingResponse[Index];

        };

        for(uint8_t Index = 0; Index < sizeof(FirstValue); Index++) {

            FirstValue[Index] ^= Random[Index];

        };

        if(!this->EncryptBlock(TemporaryKey, FirstValue, Result)) {

            return false;

        };

        for(uint8_t Index = 0; Index < sizeof(this->Address); Index++) {

            SecondValue[Index] = this->Address[Index];
            SecondValue[Index + 6] = this->PeerAddress[Index];

        };

        for(uint8_t Index = 0; Index < sizeof(SecondValue); Index++) {

            SecondValue[Index] ^= Result[Index];

        };

        return this->EncryptBlock(TemporaryKey, SecondValue, Confirm);

    };

    bool BluetoothDriver::CreateShortTermKey(const uint8_t* CentralRandom,
        const uint8_t* PeripheralRandom) {

        if(CentralRandom == nullptr || PeripheralRandom == nullptr) {

            return false;

        };

        alignas(4) uint8_t TemporaryKey[16] = {};
        alignas(4) uint8_t Cleartext[16] = {};
        for(uint8_t Index = 0; Index < 8; Index++) {

            Cleartext[Index] = CentralRandom[Index];
            Cleartext[Index + 8] = PeripheralRandom[Index];

        };

        return this->EncryptBlock(TemporaryKey, Cleartext, this->ShortTermKey);

    };

    bool BluetoothDriver::CreateSessionKey(const uint8_t* LongTermKey,
        const uint8_t* CentralDiversifier, const uint8_t* PeripheralDiversifier) {

        if(LongTermKey == nullptr || CentralDiversifier == nullptr || PeripheralDiversifier == nullptr) {

            return false;

        };

        alignas(4) uint8_t Cleartext[16] = {};
        for(uint8_t Index = 0; Index < 8; Index++) {

            Cleartext[Index] = CentralDiversifier[Index];
            Cleartext[Index + 8] = PeripheralDiversifier[Index];

        };

        return this->EncryptBlock(LongTermKey, Cleartext, this->SessionKey);

    };

    void BluetoothDriver::OnTransmitAcknowledged() {

        if(this->PendingLinkLayerID == LinkLayerControl &&
            this->PendingData[0] == LinkLayerEncryptionResponse &&
            this->Encryption == EncryptionState::ResponseSent) {

            uint8_t Response[] = {LinkLayerStartEncryptionRequest};
            this->QueueControlResponse(Response, sizeof(Response));
            this->Encryption = EncryptionState::WaitingForStartResponse;
            return;

        };

        if(this->PendingLinkLayerID == LinkLayerControl &&
            this->PendingData[0] == LinkLayerStartEncryptionResponse &&
            this->Encryption == EncryptionState::Enabled &&
            !this->UsingBondKey && this->Pairing == PairingState::WaitingForEncryption) {

            this->QueueEncryptionInformation();
            return;

        };

        if(this->PendingLinkLayerID != LinkLayerStart ||
            Read16(&this->PendingData[2]) != SecurityChannel) {

            return;

        };

        if(this->PendingData[4] == SecurityEncryptionInformation &&
            this->Pairing == PairingState::DistributingEncryptionKey) {

            this->QueueCentralIdentification();
            return;

        };

        if(this->PendingData[4] == SecurityCentralIdentification &&
            this->Pairing == PairingState::DistributingIdentification) {

            this->BondValid = true;
            this->Pairing = PairingState::Bonded;
            this->SaveBond();

        };

    };

    void BluetoothDriver::QueueSecurityRequest() {

        uint8_t Request[] = {

            SecurityRequest,
            SecurityAuthenticationBonding

        };

        this->QueueSecurityResponse(Request, sizeof(Request));
        this->SecurityRequested = true;

    };

    void BluetoothDriver::QueueEncryptionInformation() {

        alignas(4) uint8_t Response[17] = {};
        Response[0] = SecurityEncryptionInformation;
        for(uint8_t Index = 0; Index < sizeof(this->LongTermKey); Index++) {

            Response[Index + 1] = this->LongTermKey[Index];

        };

        this->QueueSecurityResponse(Response, sizeof(Response));
        this->Pairing = PairingState::DistributingEncryptionKey;

    };

    void BluetoothDriver::QueueCentralIdentification() {

        alignas(4) uint8_t Response[11] = {};
        Response[0] = SecurityCentralIdentification;
        Write16(&Response[1], this->BondDiversifier);
        for(uint8_t Index = 0; Index < sizeof(this->BondRandom); Index++) {

            Response[Index + 3] = this->BondRandom[Index];

        };

        this->QueueSecurityResponse(Response, sizeof(Response));
        this->Pairing = PairingState::DistributingIdentification;

    };

    void BluetoothDriver::QueueControlResponse(const uint8_t* Data, uint8_t Length) {

        if(Length > sizeof(this->PendingData)) {

            return;

        };

        this->PendingLinkLayerID = LinkLayerControl;
        this->PendingLength = Length;
        for(uint8_t Index = 0; Index < Length; Index++) {

            this->PendingData[Index] = Data[Index];

        };

    };

    void BluetoothDriver::QueueSecurityResponse(const uint8_t* Data, uint8_t Length) {

        if(static_cast<size_t>(Length) + 4 > sizeof(this->PendingData)) {

            return;

        };

        this->PendingLinkLayerID = LinkLayerStart;
        this->PendingLength = Length + 4;
        Write16(&this->PendingData[0], Length);
        Write16(&this->PendingData[2], SecurityChannel);

        for(uint8_t Index = 0; Index < Length; Index++) {

            this->PendingData[Index + 4] = Data[Index];

        };

    };

    void BluetoothDriver::QueueATTResponse(const uint8_t* Data, uint8_t Length) {

        if(Length > ATTMaximumTransmissionUnit || static_cast<size_t>(Length) + 4 > sizeof(this->PendingData)) {

            return;

        };

        this->PendingLinkLayerID = LinkLayerStart;
        this->PendingLength = Length + 4;
        Write16(&this->PendingData[0], Length);
        Write16(&this->PendingData[2], ATTChannel);

        for(uint8_t Index = 0; Index < Length; Index++) {

            this->PendingData[Index + 4] = Data[Index];

        };

    };

    void BluetoothDriver::QueueATTError(uint8_t Request, uint16_t Handle, uint8_t Error) {

        uint8_t Response[] = {

            ATTErrorResponse,
            Request,
            static_cast<uint8_t>(Handle),
            static_cast<uint8_t>(Handle >> 8),
            Error

        };

        this->QueueATTResponse(Response, sizeof(Response));

    };

    void BluetoothDriver::QueueKeyboardReport() {

        alignas(4) uint8_t Notification[12] = {};
        Notification[0] = ATTHandleValueNotification;
        Write16(&Notification[1], KeyboardReportHandle);

        for(uint8_t Index = 0; Index < sizeof(this->KeyboardReport); Index++) {

            Notification[Index + 3] = this->KeyboardReport[Index];

        };

        this->QueueATTResponse(Notification, 11);
        this->KeyboardReportDirty = false;

    };

    void BluetoothDriver::CreateConnectionResponse() {

        this->TransmitBuffer = {};
        this->TransmitBuffer.Header = static_cast<uint8_t>(
            this->PendingLinkLayerID |
            (this->ExpectedReceiveSequence << 2) |
            (this->TransmitSequence << 3)
        );
        this->TransmitBuffer.Length = this->PendingLength;

        for(uint8_t Index = 0; Index < this->PendingLength; Index++) {

            this->TransmitBuffer.Payload[Index] = this->PendingData[Index];

        };

    };

    uint16_t BluetoothDriver::Read16(const uint8_t* Data) {

        return static_cast<uint16_t>(Data[0]) |
            (static_cast<uint16_t>(Data[1]) << 8);

    };

    uint32_t BluetoothDriver::Read24(const uint8_t* Data) {

        return static_cast<uint32_t>(Data[0]) |
            (static_cast<uint32_t>(Data[1]) << 8) |
            (static_cast<uint32_t>(Data[2]) << 16);

    };

    uint32_t BluetoothDriver::Read32(const uint8_t* Data) {

        return static_cast<uint32_t>(Data[0]) |
            (static_cast<uint32_t>(Data[1]) << 8) |
            (static_cast<uint32_t>(Data[2]) << 16) |
            (static_cast<uint32_t>(Data[3]) << 24);

    };

    void BluetoothDriver::Write16(uint8_t* Data, uint16_t Value) {

        Data[0] = static_cast<uint8_t>(Value);
        Data[1] = static_cast<uint8_t>(Value >> 8);

    };

    uint32_t BluetoothDriver::CalculateBondChecksum(const BondRecord& Record) {

        const uint8_t* Data = reinterpret_cast<const uint8_t*>(&Record);
        uint32_t Checksum = 2166136261u;
        for(size_t Index = 0; Index < sizeof(BondRecord) - sizeof(Record.Checksum); Index++) {

            Checksum ^= Data[Index];
            Checksum *= 16777619u;

        };

        return Checksum;

    };

    void BluetoothDriver::LoadBond() {

        BondRecord Record = {};
        const volatile uint8_t* Source = reinterpret_cast<const volatile uint8_t*>(BondPage);
        uint8_t* Destination = reinterpret_cast<uint8_t*>(&Record);
        for(size_t Index = 0; Index < sizeof(Record); Index++) {

            Destination[Index] = Source[Index];

        };

        if(Record.Magic != BondMagic || Record.Version != BondVersion ||
            Record.Checksum != CalculateBondChecksum(Record)) {

            this->BondValid = false;
            return;

        };

        for(uint8_t Index = 0; Index < sizeof(this->LongTermKey); Index++) {

            this->LongTermKey[Index] = Record.LongTermKey[Index];

        };

        for(uint8_t Index = 0; Index < sizeof(this->BondRandom); Index++) {

            this->BondRandom[Index] = Record.Random[Index];

        };

        this->BondDiversifier = Record.Diversifier;
        this->BondValid = true;

    };

    void BluetoothDriver::SaveBond() {

        BondRecord Record = {};
        Record.Magic = BondMagic;
        Record.Version = BondVersion;
        for(uint8_t Index = 0; Index < sizeof(this->LongTermKey); Index++) {

            Record.LongTermKey[Index] = this->LongTermKey[Index];

        };

        for(uint8_t Index = 0; Index < sizeof(this->BondRandom); Index++) {

            Record.Random[Index] = this->BondRandom[Index];

        };

        Record.Diversifier = this->BondDiversifier;
        Record.Checksum = CalculateBondChecksum(Record);

        uint32_t InterruptState = DisableInterrupts();
        uint32_t ProtectionState = *reinterpret_cast<volatile uint32_t*>(MemoryProtectionControl);
        *reinterpret_cast<volatile uint32_t*>(MemoryProtectionControl) = 0;
        __asm__ volatile("dsb\nisb" ::: "memory");

        *reinterpret_cast<volatile uint32_t*>(NonVolatileMemoryControllerConfiguration) = 2;
        *reinterpret_cast<volatile uint32_t*>(NonVolatileMemoryControllerErasePage) = BondPage;
        while(*reinterpret_cast<volatile uint32_t*>(NonVolatileMemoryControllerReady) == 0) {};

        *reinterpret_cast<volatile uint32_t*>(NonVolatileMemoryControllerConfiguration) = 1;
        volatile uint32_t* Destination = reinterpret_cast<volatile uint32_t*>(BondPage);
        const uint32_t* Source = reinterpret_cast<const uint32_t*>(&Record);
        for(size_t Index = 0; Index < sizeof(Record) / sizeof(uint32_t); Index++) {

            Destination[Index] = Source[Index];
            while(*reinterpret_cast<volatile uint32_t*>(NonVolatileMemoryControllerReady) == 0) {};

        };

        *reinterpret_cast<volatile uint32_t*>(NonVolatileMemoryControllerConfiguration) = 0;
        *reinterpret_cast<volatile uint32_t*>(MemoryProtectionControl) = ProtectionState;
        __asm__ volatile("dsb\nisb" ::: "memory");
        RestoreInterrupts(InterruptState);

    };

};
