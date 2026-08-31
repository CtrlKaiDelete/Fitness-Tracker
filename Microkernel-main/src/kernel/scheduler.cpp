#include "Microkernel/include/kernel/scheduler.h"
#include "Microkernel/include/kernel/kernel.h"
#include "Microkernel/include/kernel/process.h"

namespace Microkernel {

    bool Scheduler::AddThread(Thread& Thread) {

        if(this->ThreadCount >= MaximumThreads) {

            return false;

        };

        for(size_t Index = 0; Index < this->ThreadCount; Index++) {

            if(this->Threads[Index] == &Thread) {

                return true;

            };

        };

        this->Threads[this->ThreadCount++] = &Thread;
        return true;

    };

    void Scheduler::RemoveThread(Thread& Thread) {

        for(size_t Index = 0; Index < this->ThreadCount; Index++) {

            if(this->Threads[Index] != &Thread) {

                continue;

            };

            this->Threads[Index] = this->Threads[this->ThreadCount - 1];
            this->Threads[--this->ThreadCount] = nullptr;
            if(this->CurrentIndex >= this->ThreadCount) {

                this->CurrentIndex = 0;

            };

            return;

        };

    };

    void Scheduler::AddJob(SchedulerJob& Job) {

        Thread* NewThread = new Thread();
        NewThread->Initialize([&Job]() {

            do {

                if(Job.Callback) {

                    Job.Callback();

                };

                if(Job.Cyclic) {

                    Kernel::New()->Scheduler.Yield();

                };

            } while(Job.Cyclic);

        });

        if(!this->AddThread(*NewThread)) {

            delete NewThread;

        };

    };

    void Scheduler::RemoveJob(SchedulerJob&) {

    };

    void Scheduler::Run() {

        if(this->Running) {

            return;

        };

        this->Running = true;
        *reinterpret_cast<volatile uint8_t*>(0xE000ED22) = 0xFF;
        this->IdleThread.Initialize(&Scheduler::Idle);
        this->Yield();
        while(this->Running) {

            __asm__ volatile("wfi");

        };

    };

    void Scheduler::Stop() {

        this->Running = false;

    };

    void Scheduler::Yield() {

        *reinterpret_cast<volatile uint32_t*>(0xE000ED04) = 1u << 28;
        __asm__ volatile("dsb\nisb" ::: "memory");

    };

    void Scheduler::Preempt() {

        if(this->Running) {

            this->Yield();

        };

    };

    uint32_t* Scheduler::Switch(uint32_t* StackPointer) {

        if(this->CurrentThread != nullptr && StackPointer != nullptr) {

            this->CurrentThread->StackPointer = StackPointer;
            if(this->CurrentThread->State == ThreadState::Running) {

                this->CurrentThread->State = ThreadState::Ready;

            };

        };

        Thread* Next = this->SelectNext();
        this->CurrentThread = Next;
        Next->State = ThreadState::Running;
        Process* Owner = Next->GetProcess();
        if(Owner != nullptr) {

            Owner->Activate();

        } else {

            this->Kernel->MemoryManager.DeactivateAddressSpace();

        };

        return Next->StackPointer;

    };

    Thread* Scheduler::GetCurrentThread() const {

        return this->CurrentThread;

    };

    bool Scheduler::IsRunning() const {

        return this->Running;

    };

    Thread* Scheduler::SelectNext() {

        for(size_t Attempt = 0; Attempt < this->ThreadCount; Attempt++) {

            size_t Index = (this->CurrentIndex + Attempt) % this->ThreadCount;
            Thread* Candidate = this->Threads[Index];
            if(Candidate != nullptr && Candidate->State == ThreadState::Ready) {

                this->CurrentIndex = (Index + 1) % this->ThreadCount;
                return Candidate;

            };

        };

        return &this->IdleThread;

    };

    void Scheduler::Idle() {

        while(true) {

            __asm__ volatile("wfi");

        };

    };

};

extern "C" uint32_t* kernel_pendsv_dispatch(uint32_t* StackPointer) {

    return Microkernel::Kernel::New()->Scheduler.Switch(StackPointer);

};
