#pragma once

#include "Microkernel/include/base/object.h"
#include "Microkernel/include/kernel/memory.h"
#include "Microkernel/include/kernel/executable.h"
#include "Microkernel/include/kernel/ipc.h"
#include "Microkernel/include/kernel/resource.h"
#include "Microkernel/include/kernel/thread.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    enum class ProcessState : uint8_t {

        Created,
        Ready,
        Running,
        Zombie,
        Terminated

    };

    class Process : public Object {

    public:

        Process();
        virtual ~Process();

        bool Initialize();
        bool Start(ExecutableEntry Entry, const ExecutableArguments& Arguments = {});
        bool Execute(ExecutableEntry Entry, const ExecutableArguments& Arguments, ExceptionFrame*& Frame);
        bool Activate();
        bool Fork(Process& Child, Thread& ParentThread);
        void Exit(int Status);
        void Terminate();

        Page* AllocateMemory(PagePermission Permissions, bool Pinned = false);
        bool ReleaseMemory(Page& Page);

        uint32_t GetIdentifier() const;
        ProcessState GetState() const;
        AddressSpace& GetAddressSpace();
        void* GetStackTop() const;
        bool ValidatePointer(const void* Pointer, size_t Size, PagePermission Permission) const;
        Process* GetParent() const;
        int GetExitStatus() const;
        Thread& GetMainThread();
        HandleTable& GetHandles();
        WaitQueue& GetChildWaitQueue();

    private:

        bool BuildArgumentStack(
            const ExecutableArguments& Arguments,
            void*& StackTop,
            uintptr_t& ArgumentVector,
            uintptr_t& EnvironmentVector
        );

        static inline uint32_t NextIdentifier = 1;

        uint32_t Identifier = 0;
        ProcessState State = ProcessState::Created;
        AddressSpace ProcessAddressSpace;
        Page* OwnedPages[PageCount] = {};
        size_t OwnedPageCount = 0;
        Page* StackPage = nullptr;
        Process* Parent = nullptr;
        int ExitStatus = 0;
        Thread MainThread;
        HandleTable Handles;
        WaitQueue ChildWaitQueue;

        friend class ProcessTable;

    };

    class ProcessTable : public Object {

    public:

        static constexpr size_t MaximumProcesses = 8;
        Process* Create(Process* Parent = nullptr);
        Process* ForkCurrent();
        Process* Find(uint32_t Identifier);
        Process* GetCurrent() const;
        Process* FindZombieChild(Process& Parent);
        bool Reap(Process& Process);

    private:

        Process Processes[MaximumProcesses] = {};
        bool Used[MaximumProcesses] = {};

    };

};
