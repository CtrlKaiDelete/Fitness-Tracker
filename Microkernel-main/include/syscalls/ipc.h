#pragma once

#include "Microkernel/include/syscalls/syscall.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    struct IPCPacket {

        static constexpr size_t MaximumData = 64;
        uint8_t Data[MaximumData] = {};
        size_t Size = 0;
        uint32_t Handle = 0;
        uint32_t SenderIdentifier = 0;

    };

    namespace IPCSyscallNumber {

        constexpr uint32_t CreateEndpoint = 1;
        constexpr uint32_t Send = 2;
        constexpr uint32_t Receive = 3;
        constexpr uint32_t CloseHandle = 4;
        constexpr uint32_t RegisterService = 16;
        constexpr uint32_t ConnectService = 17;

    };

    class CreateEndpointSyscall : public Syscall {

    public:

        CreateEndpointSyscall() {

            this->SyscallNumber = IPCSyscallNumber::CreateEndpoint;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class IPCSendSyscall : public Syscall {

    public:

        IPCSendSyscall() {

            this->SyscallNumber = IPCSyscallNumber::Send;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class IPCReceiveSyscall : public Syscall {

    public:

        IPCReceiveSyscall() {

            this->SyscallNumber = IPCSyscallNumber::Receive;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class CloseHandleSyscall : public Syscall {

    public:

        CloseHandleSyscall() {

            this->SyscallNumber = IPCSyscallNumber::CloseHandle;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class RegisterServiceSyscall : public Syscall {

    public:

        RegisterServiceSyscall() {

            this->SyscallNumber = IPCSyscallNumber::RegisterService;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    class ConnectServiceSyscall : public Syscall {

    public:

        ConnectServiceSyscall() {

            this->SyscallNumber = IPCSyscallNumber::ConnectService;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

};
