#include "Microkernel/include/syscalls/ipc.h"
#include "Microkernel/include/kernel/ipc.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    uintptr_t CreateEndpointSyscall::Execute(uintptr_t, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        IPCEndpoint* Endpoint = this->Kernel->IPC.CreateEndpoint(Current->GetIdentifier());
        if(Endpoint == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        uint32_t Handle = Current->GetHandles().Add(
            *Endpoint,
            Capability::Read | Capability::Write | Capability::Transfer | Capability::Manage
        );
        if(Handle == 0) {

            return static_cast<uintptr_t>(-1);

        };

        return Handle;

    };

    uintptr_t IPCSendSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto Packet = reinterpret_cast<const IPCPacket*>(Arg1);
        if(Current == nullptr || !this->ValidateRead(Packet, sizeof(IPCPacket)) ||
            Packet->Size > IPCMessage::MaximumData) {

            return static_cast<uintptr_t>(-1);

        };

        auto Endpoint = static_cast<IPCEndpoint*>(Current->GetHandles().Get(
            Arg0,
            Capability::Write,
            ResourceType::Endpoint
        ));
        if(Endpoint == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        IPCMessage Message = {};
        Message.Size = Packet->Size;
        Message.SenderIdentifier = Current->GetIdentifier();
        for(size_t Index = 0; Index < Message.Size; Index++) {

            Message.Data[Index] = Packet->Data[Index];

        };

        Process* TransferOwner = nullptr;
        if(Packet->Handle != 0) {

            TransferOwner = this->Kernel->Processes.Find(Endpoint->GetOwnerIdentifier());
            if(TransferOwner == nullptr || !Current->GetHandles().DuplicateTo(
                Packet->Handle,
                TransferOwner->GetHandles(),
                Capability::Read | Capability::Write,
                Message.Handle
            )) {

                return static_cast<uintptr_t>(-1);

            };

        };

        if(Endpoint->Send(Message) != IPCResult::Success) {

            if(TransferOwner != nullptr && Message.Handle != 0) {

                TransferOwner->GetHandles().Close(Message.Handle);

            };

            return static_cast<uintptr_t>(-1);

        };

        return Message.Size;

    };

    uintptr_t IPCReceiveSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto Packet = reinterpret_cast<IPCPacket*>(Arg1);
        bool Block = Arg2 != 0;
        if(Current == nullptr || !this->ValidateWrite(Packet, sizeof(IPCPacket))) {

            return static_cast<uintptr_t>(-1);

        };

        auto Endpoint = static_cast<IPCEndpoint*>(Current->GetHandles().Get(
            Arg0,
            Capability::Read,
            ResourceType::Endpoint
        ));
        if(Endpoint == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        IPCMessage Message = {};
        if(Endpoint->Receive(Message, this->Kernel->Scheduler.GetCurrentThread(), Block) != IPCResult::Success) {

            return static_cast<uintptr_t>(-1);

        };

        for(size_t Index = 0; Index < Message.Size; Index++) {

            Packet->Data[Index] = Message.Data[Index];

        };

        Packet->Size = Message.Size;
        Packet->Handle = Message.Handle;
        Packet->SenderIdentifier = Message.SenderIdentifier;

        return Message.Size;

    };

    uintptr_t CloseHandleSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        return Current != nullptr && Current->GetHandles().Close(Arg0) ? 0 : static_cast<uintptr_t>(-1);

    };

    uintptr_t RegisterServiceSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto Name = reinterpret_cast<const char*>(Arg0);
        if(Current == nullptr || !this->ValidateString(Name, IPCManager::MaximumServiceName)) {

            return static_cast<uintptr_t>(-1);

        };

        IPCEndpoint* Endpoint = this->Kernel->IPC.RegisterService(Name, Current->GetIdentifier());
        if(Endpoint == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        uint32_t Handle = Current->GetHandles().Add(
            *Endpoint,
            Capability::Read | Capability::Write | Capability::Transfer | Capability::Manage
        );
        if(Handle == 0) {

            this->Kernel->IPC.RemoveServices(Current->GetIdentifier());
            return static_cast<uintptr_t>(-1);

        };

        return Handle;

    };

    uintptr_t ConnectServiceSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto Name = reinterpret_cast<const char*>(Arg0);
        if(Current == nullptr || !this->ValidateString(Name, IPCManager::MaximumServiceName)) {

            return static_cast<uintptr_t>(-1);

        };

        IPCEndpoint* Endpoint = this->Kernel->IPC.ConnectService(Name);
        if(Endpoint == nullptr) {

            return static_cast<uintptr_t>(-1);

        };

        uint32_t Handle = Current->GetHandles().Add(*Endpoint, Capability::Write);
        return Handle == 0 ? static_cast<uintptr_t>(-1) : Handle;

    };

};
