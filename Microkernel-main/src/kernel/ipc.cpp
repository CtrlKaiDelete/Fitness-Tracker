#include "Microkernel/include/kernel/ipc.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    IPCEndpoint::IPCEndpoint(uint32_t OwnerIdentifier) :
        Resource(ResourceType::Endpoint), OwnerIdentifier(OwnerIdentifier) {

    };

    bool WaitQueue::Wait(Thread& Thread) {

        if(this->Count >= MaximumWaiters) {

            return false;

        };

        for(size_t Index = 0; Index < this->Count; Index++) {

            if(this->Waiters[Index] == &Thread) {

                return true;

            };

        };

        this->Waiters[this->Count++] = &Thread;
        Thread.Block(this);
        return true;

    };

    Thread* WaitQueue::WakeOne() {

        if(this->Count == 0) {

            return nullptr;

        };

        Thread* Result = this->Waiters[0];
        for(size_t Index = 1; Index < this->Count; Index++) {

            this->Waiters[Index - 1] = this->Waiters[Index];

        };

        this->Waiters[--this->Count] = nullptr;
        Result->Wake();
        return Result;

    };

    void WaitQueue::WakeAll() {

        while(this->Count > 0) {

            this->WakeOne();

        };

    };

    IPCResult IPCEndpoint::Send(const IPCMessage& Message) {

        if(Message.Size > IPCMessage::MaximumData) {

            return IPCResult::InvalidArgument;

        };

        if(this->Count >= MaximumMessages) {

            return IPCResult::QueueFull;

        };

        this->Messages[this->Tail] = Message;
        this->Tail = (this->Tail + 1) % MaximumMessages;
        this->Count++;
        this->Receivers.WakeOne();
        return IPCResult::Success;

    };

    IPCResult IPCEndpoint::Receive(IPCMessage& Message, Thread* Receiver, bool Block) {

        if(this->Count == 0) {

            if(Block && Receiver != nullptr && this->Receivers.Wait(*Receiver)) {

                Kernel::New()->Scheduler.Yield();

            };

            return IPCResult::WouldBlock;

        };

        Message = this->Messages[this->Head];
        this->Messages[this->Head] = {};
        this->Head = (this->Head + 1) % MaximumMessages;
        
        this->Count--;
        return IPCResult::Success;

    };

    WaitQueue& IPCEndpoint::GetWaitQueue() {

        return this->Receivers;

    };

    uint32_t IPCEndpoint::GetOwnerIdentifier() const {

        return this->OwnerIdentifier;

    };

    IPCEndpoint* IPCManager::CreateEndpoint(uint32_t OwnerIdentifier) {

        for(auto& Endpoint : this->Endpoints) {

            if(Endpoint.GetReferenceCount() == 0) {

                Endpoint = IPCEndpoint(OwnerIdentifier);
                return &Endpoint;

            };

        };

        return nullptr;

    };

    IPCEndpoint* IPCManager::RegisterService(const char* Name, uint32_t OwnerIdentifier) {

        if(Name == nullptr || OwnerIdentifier == 0) {

            return nullptr;

        };

        size_t Length = 0;
        while(Name[Length] != '\0' && Length < MaximumServiceName) {

            Length++;

        };

        if(Length == 0 || Length == MaximumServiceName || this->ConnectService(Name) != nullptr) {

            return nullptr;

        };

        for(auto& Service : this->Services) {

            if(Service.Endpoint != nullptr) {

                continue;

            };

            IPCEndpoint* Endpoint = this->CreateEndpoint(OwnerIdentifier);
            if(Endpoint == nullptr) {

                return nullptr;

            };

            for(size_t Index = 0; Index <= Length; Index++) {

                Service.Name[Index] = Name[Index];

            };

            Service.Endpoint = Endpoint;
            Service.OwnerIdentifier = OwnerIdentifier;
            Endpoint->Retain();
            return Endpoint;

        };

        return nullptr;

    };

    IPCEndpoint* IPCManager::ConnectService(const char* Name) {

        if(Name == nullptr) {

            return nullptr;

        };

        for(auto& Service : this->Services) {

            if(Service.Endpoint == nullptr) {

                continue;

            };

            size_t Index = 0;
            while(Service.Name[Index] == Name[Index] && Name[Index] != '\0') {

                Index++;

            };

            if(Service.Name[Index] == '\0' && Name[Index] == '\0') {

                return Service.Endpoint;

            };

        };

        return nullptr;

    };

    void IPCManager::RemoveServices(uint32_t OwnerIdentifier) {

        for(auto& Service : this->Services) {

            if(Service.Endpoint == nullptr || Service.OwnerIdentifier != OwnerIdentifier) {

                continue;

            };

            Service.Endpoint->Release();
            Service = {};

        };

    };

};
