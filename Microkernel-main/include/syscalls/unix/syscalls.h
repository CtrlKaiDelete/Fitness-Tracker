#pragma once

#include "Microkernel/include/syscalls/syscall.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    namespace UnixSyscallNumber {

        constexpr uint32_t Chown = 32;
        constexpr uint32_t Close = 33;
        constexpr uint32_t Execve = 34;
        constexpr uint32_t Exit = 35;
        constexpr uint32_t Fork = 36;
        constexpr uint32_t Fstat = 37;
        constexpr uint32_t GetEntropy = 38;
        constexpr uint32_t GetPid = 39;
        constexpr uint32_t GetTimeOfDay = 40;
        constexpr uint32_t IsATty = 41;
        constexpr uint32_t Kill = 42;
        constexpr uint32_t Link = 43;
        constexpr uint32_t Lseek = 44;
        constexpr uint32_t Open = 45;
        constexpr uint32_t Read = 46;
        constexpr uint32_t ReadLink = 47;
        constexpr uint32_t Sbrk = 48;
        constexpr uint32_t Stat = 49;
        constexpr uint32_t Symlink = 50;
        constexpr uint32_t Times = 51;
        constexpr uint32_t Unlink = 52;
        constexpr uint32_t Wait = 53;
        constexpr uint32_t Write = 54;

    };

    class UnixSyscall : public Syscall {

    protected:

        static uintptr_t Fail(int Error);
        static bool IsStandardDescriptor(int FileDescriptor);
        bool CopyUserString(const char* Source, char* Destination, size_t Capacity) const;

    };

    /*
     * I had to finally whip out the macro once it got this ridiculous..
     */

    #define DECLARE_UNIX_SYSCALL(Name) \
        class Name##Syscall : public UnixSyscall { \
        public: \
            Name##Syscall() { \
                \
                this->SyscallNumber = UnixSyscallNumber::Name; \
                \
            }; \
            uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override; \
            \
        }

    DECLARE_UNIX_SYSCALL(Chown);
    DECLARE_UNIX_SYSCALL(Close);
    DECLARE_UNIX_SYSCALL(Execve);
    DECLARE_UNIX_SYSCALL(Exit);
    DECLARE_UNIX_SYSCALL(Fork);
    DECLARE_UNIX_SYSCALL(Fstat);
    DECLARE_UNIX_SYSCALL(GetEntropy);
    DECLARE_UNIX_SYSCALL(GetPid);
    DECLARE_UNIX_SYSCALL(GetTimeOfDay);
    DECLARE_UNIX_SYSCALL(IsATty);
    DECLARE_UNIX_SYSCALL(Kill);
    DECLARE_UNIX_SYSCALL(Link);
    DECLARE_UNIX_SYSCALL(Lseek);
    DECLARE_UNIX_SYSCALL(Open);
    DECLARE_UNIX_SYSCALL(Read);
    DECLARE_UNIX_SYSCALL(ReadLink);

    class SbrkSyscall : public UnixSyscall {

    public:

        SbrkSyscall() {

            this->SyscallNumber = UnixSyscallNumber::Sbrk;

        };
        uintptr_t Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) override;

    };

    DECLARE_UNIX_SYSCALL(Stat);
    DECLARE_UNIX_SYSCALL(Symlink);
    DECLARE_UNIX_SYSCALL(Times);
    DECLARE_UNIX_SYSCALL(Unlink);
    DECLARE_UNIX_SYSCALL(Wait);
    DECLARE_UNIX_SYSCALL(Write);

#undef DECLARE_UNIX_SYSCALL

    uintptr_t InvokeUnixSyscall(uint32_t Number, uintptr_t Arg0 = 0, uintptr_t Arg1 = 0, uintptr_t Arg2 = 0);

};
