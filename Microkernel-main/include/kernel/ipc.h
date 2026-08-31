#pragma once

#include "Microkernel/include/kernel/resource.h"
#include "Microkernel/include/kernel/thread.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    enum class IPCResult : uint8_t {

        Success,
        WouldBlock,
        QueueFull,
        InvalidArgument

    };

    struct IPCMessage {

        static constexpr size_t MaximumData = 64;
        uint8_t Data[MaximumData] = {};
        size_t Size = 0;
        uint32_t Handle = 0;
        uint32_t SenderIdentifier = 0;

    };

    class WaitQueue {

    public:

        static constexpr size_t MaximumWaiters = 16;
        bool Wait(Thread& Thread);
        Thread* WakeOne();
        void WakeAll();

    private:

        Thread* Waiters[MaximumWaiters] = {};
        size_t Count = 0;

    };

    class IPCEndpoint : public Resource {

    public:

        static constexpr size_t MaximumMessages = 8;
        IPCEndpoint(uint32_t OwnerIdentifier = 0);
        IPCResult Send(const IPCMessage& Message);
        IPCResult Receive(IPCMessage& Message, Thread* Receiver = nullptr, bool Block = true);
        WaitQueue& GetWaitQueue();
        uint32_t GetOwnerIdentifier() const;

    private:

        IPCMessage Messages[MaximumMessages] = {};
        size_t Head = 0;
        size_t Tail = 0;
        size_t Count = 0;
        WaitQueue Receivers;
        uint32_t OwnerIdentifier = 0;

    };

    class IPCManager : public Object {

    public:

        static constexpr size_t MaximumEndpoints = 8;
        static constexpr size_t MaximumServices = 8;
        static constexpr size_t MaximumServiceName = 48;

        IPCEndpoint* CreateEndpoint(uint32_t OwnerIdentifier);
        IPCEndpoint* RegisterService(const char* Name, uint32_t OwnerIdentifier);
        IPCEndpoint* ConnectService(const char* Name);
        void RemoveServices(uint32_t OwnerIdentifier);

    private:

        struct Service {

            char Name[MaximumServiceName] = {};
            IPCEndpoint* Endpoint = nullptr;
            uint32_t OwnerIdentifier = 0;

        };

        IPCEndpoint Endpoints[MaximumEndpoints] = {};
        Service Services[MaximumServices] = {};

    };

};
