#pragma once

#include "Microkernel/include/drivers/driver.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    class BluetoothDriver : public Driver {

    public:

        static constexpr const char* DeviceName = "MicroOS";

        void Update();
        void SetKeyboardReport(uint8_t Modifier, uint8_t KeyCode);

        [[nodiscard]] bool IsAdvertising() const;
        [[nodiscard]] bool IsConnected() const;
        [[nodiscard]] const uint8_t* GetAddress() const;

    protected:

        void OnStart() override;
        void OnStop() override;
        void OnPanic() override;

    private:

        enum class LinkState {

            Stopped,
            Advertising,
            Connecting,
            Connected

        };

        enum class PairingState {

            Idle,
            WaitingForConfirm,
            WaitingForRandom,
            WaitingForEncryption,
            DistributingEncryptionKey,
            DistributingIdentification,
            Bonded

        };

        enum class EncryptionState {

            Disabled,
            ResponseSent,
            WaitingForStartResponse,
            Enabled

        };

        static constexpr size_t ChannelMapSize = 5;

        struct alignas(4) RadioPacket {

            uint8_t Header = 0;
            uint8_t Length = 0;
            uint8_t Reserved = 0;
            uint8_t Payload[255] = {};

        };

        struct alignas(4) ECBData {

            uint8_t Key[16] = {};
            uint8_t Cleartext[16] = {};
            uint8_t Ciphertext[16] = {};

        };

        struct alignas(4) CCMConfigurationData {

            uint8_t Key[16] = {};
            uint8_t PacketCounter[8] = {};
            uint8_t Direction = 0;
            uint8_t InitializationVector[8] = {};

        };

        struct alignas(4) BondRecord {

            uint32_t Magic = 0;
            uint32_t Version = 0;
            uint8_t LongTermKey[16] = {};
            uint8_t Random[8] = {};
            uint16_t Diversifier = 0;
            uint16_t Reserved = 0;
            uint32_t Checksum = 0;

        };

        void StartTimer();
        void StopTimer();
        [[nodiscard]] uint32_t GetMicroseconds() const;

        bool StartHighFrequencyClock();
        void ConfigureCommonRadio();
        void ConfigureAdvertisingRadio(uint8_t Channel);
        void ConfigureConnectionRadio(uint8_t Channel);
        void DisableRadio();

        bool TransmitPacket(RadioPacket& Packet, uint32_t AirTime = 0);
        bool ReceivePacket(RadioPacket& Packet, uint32_t Timeout, uint32_t& AddressTime, uint32_t& EndTime);

        bool FillRandom(uint8_t* Data, size_t Length);
        bool EncryptBlock(const uint8_t* Key, const uint8_t* Cleartext, uint8_t* Ciphertext);
        bool EncryptLinkPacket(const RadioPacket& Input, RadioPacket& Output, uint64_t Counter, bool CentralDirection);
        bool DecryptLinkPacket(const RadioPacket& Input, RadioPacket& Output, uint64_t Counter, bool CentralDirection);
        bool ProcessCCM(const RadioPacket& Input, RadioPacket& Output, uint64_t Counter,
            bool CentralDirection, bool Decrypt);

        void CreateAddress();
        void CreateAdvertisement();
        void CreateScanResponse();
        void Advertise();
        void HandleAdvertisingPacket(uint32_t EndTime);

        bool BeginConnection(uint32_t EndTime);
        void UpdateConnection();
        bool HandleConnectionEvent(uint8_t Channel, uint32_t Timeout, bool FirstEvent);
        void EndConnection();

        [[nodiscard]] uint8_t GetNextDataChannel();
        [[nodiscard]] uint8_t RemapDataChannel(uint8_t Channel) const;
        [[nodiscard]] bool IsDataChannelEnabled(uint8_t Channel) const;
        [[nodiscard]] uint8_t GetUsedDataChannelCount() const;

        void HandleDataPacket();
        void HandleControlPacket();
        void HandleL2CAPPacket();
        void HandleSecurityPacket(const uint8_t* Request, uint8_t Length);
        void HandleATTPacket(const uint8_t* Request, uint8_t Length);
        bool ReadAttribute(uint16_t Handle, uint16_t Offset, uint8_t* Data, uint8_t& Length) const;
        bool WriteAttribute(uint16_t Handle, const uint8_t* Data, uint8_t Length);

        bool CreateConfirm(const uint8_t* Random, uint8_t* Confirm);
        bool CreateShortTermKey(const uint8_t* CentralRandom, const uint8_t* PeripheralRandom);
        bool CreateSessionKey(const uint8_t* LongTermKey, const uint8_t* CentralDiversifier,
            const uint8_t* PeripheralDiversifier);
        void OnTransmitAcknowledged();
        void QueueSecurityRequest();
        void QueueEncryptionInformation();
        void QueueCentralIdentification();

        void QueueControlResponse(const uint8_t* Data, uint8_t Length);
        void QueueSecurityResponse(const uint8_t* Data, uint8_t Length);
        void QueueATTResponse(const uint8_t* Data, uint8_t Length);
        void QueueATTError(uint8_t Request, uint16_t Handle, uint8_t Error);
        void QueueKeyboardReport();
        void CreateConnectionResponse();

        static uint16_t Read16(const uint8_t* Data);
        static uint32_t Read24(const uint8_t* Data);
        static uint32_t Read32(const uint8_t* Data);
        static void Write16(uint8_t* Data, uint16_t Value);
        static uint32_t CalculateBondChecksum(const BondRecord& Record);

        void LoadBond();
        void SaveBond();

        LinkState State = LinkState::Stopped;

        uint8_t Address[6] = {};
        uint8_t PeerAddress[6] = {};
        uint8_t PeerAddressType = 0;
        RadioPacket AdvertisementPacket = {};
        RadioPacket ScanResponsePacket = {};
        RadioPacket ReceiveBuffer = {};
        RadioPacket DecryptedReceiveBuffer = {};
        RadioPacket TransmitBuffer = {};
        RadioPacket EncryptedTransmitBuffer = {};

        uint32_t NextAdvertisement = 0;
        uint8_t AdvertisingChannelIndex = 0;

        uint32_t AccessAddress = 0;
        uint32_t CRCInitialization = 0;
        uint8_t WindowSize = 0;
        uint16_t WindowOffset = 0;
        uint16_t ConnectionInterval = 0;
        uint16_t ConnectionLatency = 0;
        uint16_t SupervisionTimeout = 0;
        alignas(4) uint8_t ChannelMap[8] = {};
        uint8_t HopIncrement = 0;
        uint8_t LastUnmappedChannel = 0;

        uint32_t FirstWindowStart = 0;
        uint32_t FirstWindowEnd = 0;
        uint32_t NextConnectionAnchor = 0;
        uint32_t LastPacketTime = 0;
        uint16_t ConnectionEventCounter = 0;

        uint8_t ExpectedReceiveSequence = 0;
        uint8_t TransmitSequence = 0;
        bool TransmitOutstanding = false;
        uint8_t PendingLinkLayerID = 1;
        uint8_t PendingLength = 0;
        alignas(4) uint8_t PendingData[28] = {};

        bool ConnectionUpdatePending = false;
        uint8_t PendingWindowSize = 0;
        uint16_t PendingWindowOffset = 0;
        uint16_t PendingConnectionInterval = 0;
        uint16_t PendingConnectionLatency = 0;
        uint16_t PendingSupervisionTimeout = 0;
        uint16_t ConnectionUpdateInstant = 0;

        bool ChannelMapUpdatePending = false;
        alignas(4) uint8_t PendingChannelMap[8] = {};
        uint16_t ChannelMapUpdateInstant = 0;

        alignas(4) uint8_t KeyboardReport[8] = {};
        uint8_t ProtocolMode = 1;
        bool KeyboardNotificationsEnabled = false;
        bool BatteryNotificationsEnabled = false;
        bool KeyboardReportDirty = false;

        PairingState Pairing = PairingState::Idle;
        EncryptionState Encryption = EncryptionState::Disabled;
        bool SecurityRequested = false;
        bool ReceiveEncryptionEnabled = false;
        bool TransmitEncryptionEnabled = false;
        bool UsingBondKey = false;
        bool BondValid = false;
        uint8_t PairingRequest[7] = {};
        uint8_t PairingResponse[7] = {};
        uint8_t CentralConfirm[16] = {};
        uint8_t CentralRandom[16] = {};
        uint8_t PeripheralRandom[16] = {};
        uint8_t ShortTermKey[16] = {};
        uint8_t LongTermKey[16] = {};
        uint8_t BondRandom[8] = {};
        uint16_t BondDiversifier = 0;
        uint8_t SessionKey[16] = {};
        uint8_t SessionInitializationVector[8] = {};
        uint64_t ReceivePacketCounter = 0;
        uint64_t TransmitPacketCounter = 0;
        uint64_t OutstandingTransmitPacketCounter = 0;
        alignas(4) ECBData EncryptionBlock = {};
        alignas(4) CCMConfigurationData CCMConfiguration = {};
        alignas(4) uint8_t CCMScratch[44] = {};

    };

};
