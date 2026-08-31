#include "Microkernel/include/kernel/thread.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cstring>

namespace Microkernel {

    bool Thread::Initialize(Entry Callback, Process* Owner, bool User) {

        if(this->State != ThreadState::Created || !Callback) {

            return false;

        };

        this->Callback = Callback;
        this->Owner = Owner;
        this->User = User;

        uint32_t* StackTop = reinterpret_cast<uint32_t*>(this->Stack + sizeof(this->Stack));
        *(--StackTop) = 0x01000000;
        *(--StackTop) = reinterpret_cast<uintptr_t>(&Thread::Start);
        for(size_t Register = 0; Register < 6; Register++) {

            *(--StackTop) = 0;

        };

        for(size_t Register = 0; Register < 8; Register++) {

            *(--StackTop) = 0;

        };

        *(--StackTop) = 0xFFFFFFFD;
        *(--StackTop) = User ? 3u : 2u;
        this->StackPointer = StackTop;
        this->State = ThreadState::Ready;
        return true;

    };

    bool Thread::InitializeUser(
        uintptr_t Entry,
        uintptr_t ReturnAddress,
        void* StackTop,
        Process& Owner,
        uintptr_t Arg0,
        uintptr_t Arg1,
        uintptr_t Arg2
    ) {

        if(this->State != ThreadState::Created || Entry == 0 || ReturnAddress == 0 || StackTop == nullptr) {

            return false;

        };

        this->Owner = &Owner;
        this->User = true;
        uint32_t* Context = static_cast<uint32_t*>(StackTop);
        *(--Context) = 0x01000000;
        *(--Context) = Entry;
        *(--Context) = ReturnAddress;
        *(--Context) = 0;
        *(--Context) = 0;
        *(--Context) = Arg2;
        *(--Context) = Arg1;
        *(--Context) = Arg0;

        for(size_t Register = 0; Register < 8; Register++) {

            *(--Context) = 0;

        };

        *(--Context) = 0xFFFFFFFD;
        *(--Context) = 3;
        this->StackPointer = Context;
        this->State = ThreadState::Ready;
        return true;

    };

    bool Thread::Clone(
        Thread& Parent,
        Process* Owner,
        const ExceptionFrame* Frame,
        const uint32_t* Registers,
        void* SourceStack,
        void* DestinationStack,
        size_t StackLength
    ) {

        if(this->State != ThreadState::Created || Parent.StackPointer == nullptr) {

            return false;

        };

        uint8_t* Source = SourceStack == nullptr ? Parent.Stack : static_cast<uint8_t*>(SourceStack);
        uint8_t* Destination = DestinationStack == nullptr ? this->Stack : static_cast<uint8_t*>(DestinationStack);
        size_t Length = StackLength == 0 ? sizeof(this->Stack) : StackLength;
        std::memcpy(Destination, Source, Length);
        ptrdiff_t Offset = reinterpret_cast<uint8_t*>(Parent.StackPointer) - Source;
        if(Frame != nullptr) {

            ptrdiff_t FrameOffset = reinterpret_cast<const uint8_t*>(Frame) - Source;
            if(
                FrameOffset < 10 * static_cast<ptrdiff_t>(sizeof(uint32_t)) ||
                FrameOffset + static_cast<ptrdiff_t>(sizeof(ExceptionFrame)) > static_cast<ptrdiff_t>(Length)
            ) {

                return false;

            };

            auto ChildFrame = reinterpret_cast<ExceptionFrame*>(Destination + FrameOffset);
            *ChildFrame = *Frame;
            ChildFrame->R0 = 0;
            this->StackPointer = reinterpret_cast<uint32_t*>(ChildFrame) - 10;
            this->StackPointer[0] = Parent.User ? 3u : 2u;
            this->StackPointer[1] = 0xFFFFFFFD;
            intptr_t Delta = Destination - Source;
            auto Relocate = [Source, Length, Delta](uint32_t Value) {

                uintptr_t Address = Value;
                return
                    Address >= reinterpret_cast<uintptr_t>(Source) &&
                    Address < reinterpret_cast<uintptr_t>(Source) + Length ?
                    static_cast<uint32_t>(Address + Delta) :
                    Value;

            };

            for(size_t Register = 0; Register < 8; Register++) {

                this->StackPointer[2 + Register] = Relocate(Registers == nullptr ? 0 : Registers[Register]);

            };

            ChildFrame->R1 = Relocate(ChildFrame->R1);
            ChildFrame->R2 = Relocate(ChildFrame->R2);
            ChildFrame->R3 = Relocate(ChildFrame->R3);
            ChildFrame->R12 = Relocate(ChildFrame->R12);

        } else {

            this->StackPointer = reinterpret_cast<uint32_t*>(Destination + Offset);
            this->StackPointer[10] = 0;

        };
        this->Callback = Parent.Callback;
        this->Owner = Owner;
        this->User = Parent.User;
        this->State = ThreadState::Ready;
        return true;

    };

    void Thread::Block(WaitQueue* Queue) {

        this->Queue = Queue;
        this->State = ThreadState::Blocked;

    };

    void Thread::Wake() {

        if(this->State == ThreadState::Blocked) {

            this->Queue = nullptr;
            this->State = ThreadState::Ready;

        };

    };

    void Thread::Terminate() {

        this->Queue = nullptr;
        this->State = ThreadState::Terminated;

    };

    uint32_t* Thread::GetStackPointer() const {

        return this->StackPointer;

    };

    void Thread::SetStackPointer(uint32_t* StackPointer) {

        this->StackPointer = StackPointer;

    };

    ThreadState Thread::GetState() const {

        return this->State;

    };

    Process* Thread::GetProcess() const {

        return this->Owner;

    };

    WaitQueue* Thread::GetWaitQueue() const {

        return this->Queue;

    };

    bool Thread::IsUser() const {

        return this->User;

    };

    void Thread::Start() {

        Thread* Current = Kernel::New()->Scheduler.GetCurrentThread();
        if(Current != nullptr && Current->Callback) {

            Current->Callback();

        };

        if(Current != nullptr) {

            Current->Terminate();

        };

        Kernel::New()->Scheduler.Yield();
        while(true) {

            __asm__ volatile("wfi");

        };

    };

};
