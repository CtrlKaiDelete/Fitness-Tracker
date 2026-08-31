#pragma once

#include "Microkernel/include/syscalls/syscall.h"
#include "Microkernel/include/libkern/exceptionframe.h"

#include "Microkernel/include/base/object.h"

#include <array>
#include <cstddef>
#include <new>

namespace Microkernel {

    class SyscallManager : public Object {

    public:

        template<typename T>
        T& RegisterSyscall() {

            for(Syscall* Call : this->Syscalls) {

                T* Existing = dynamic_cast<T*>(Call);
                if(Existing != nullptr) {

                    return *Existing;

                };

            };

            alignas(T) static std::byte Storage[sizeof(T)];
            static T* Call = nullptr;
            if(Call == nullptr) {

                Call = new (Storage) T();

            };

            if(Call->SyscallNumber < this->Syscalls.size()) {

                this->Syscalls[Call->SyscallNumber] = Call;

            };

            return *Call;

        };

        template<typename T>
        void UnregisterSyscall() {

            for(Syscall*& Call : this->Syscalls) {

                if(dynamic_cast<T*>(Call) != nullptr) {

                    Call = nullptr;
                    return;

                };

            };

        };

        uintptr_t Execute(uint32_t Number, uintptr_t Arg0 = 0, uintptr_t Arg1 = 0, uintptr_t Arg2 = 0);
        void HandleExceptionFrame(ExceptionFrame* E);
        bool ReplaceCurrentExceptionFrame(ExceptionFrame* Frame);
        const ExceptionFrame* GetCurrentFrame() const;
        const uint32_t* GetCurrentRegisters() const;
        uint32_t GetCurrentExceptionReturn() const;

        SyscallManager() = default;
        virtual ~SyscallManager() = default;

    private:

        friend void SetSyscallContext(
            SyscallManager& Manager,
            const uint32_t* Registers,
            uint32_t ExceptionReturn
        );

        static constexpr size_t MaximumSyscalls = 64;
        std::array<Syscall*, MaximumSyscalls> Syscalls = {};
        ExceptionFrame* CurrentFrame = nullptr;
        const uint32_t* CurrentRegisters = nullptr;
        uint32_t CurrentExceptionReturn = 0;

    };

};
