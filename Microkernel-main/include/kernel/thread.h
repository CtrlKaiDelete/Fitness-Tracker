#pragma once

#include "Microkernel/include/base/object.h"
#include "Microkernel/include/libkern/exceptionframe.h"

#include <cstddef>
#include <cstdint>
#include <functional>

namespace Microkernel {

    class Process;
    class WaitQueue;

    enum class ThreadState : uint8_t {

        Created,
        Ready,
        Running,
        Blocked,
        Terminated

    };

    class Thread : public Object {

    public:

        static constexpr size_t StackSize = 1024;
        using Entry = std::function<void()>;

        Thread() = default;
        virtual ~Thread() = default;

        bool Initialize(Entry Callback, Process* Owner = nullptr, bool User = false);
        bool InitializeUser(
            uintptr_t Entry,
            uintptr_t ReturnAddress,
            void* StackTop,
            Process& Owner,
            uintptr_t Arg0 = 0,
            uintptr_t Arg1 = 0,
            uintptr_t Arg2 = 0
        );
        bool Clone(
            Thread& Parent,
            Process* Owner,
            const ExceptionFrame* Frame = nullptr,
            const uint32_t* Registers = nullptr,
            void* SourceStack = nullptr,
            void* DestinationStack = nullptr,
            size_t StackLength = 0
        );
        void Block(WaitQueue* Queue = nullptr);
        void Wake();
        void Terminate();
        uint32_t* GetStackPointer() const;
        void SetStackPointer(uint32_t* StackPointer);
        ThreadState GetState() const;
        Process* GetProcess() const;
        WaitQueue* GetWaitQueue() const;
        bool IsUser() const;

    private:

        static void Start();

        alignas(8) uint8_t Stack[StackSize] = {};
        uint32_t* StackPointer = nullptr;
        Entry Callback;
        Process* Owner = nullptr;
        WaitQueue* Queue = nullptr;
        ThreadState State = ThreadState::Created;
        bool User = false;

        friend class Scheduler;

    };

};
