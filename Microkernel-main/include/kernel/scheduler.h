#pragma once

#include "Microkernel/include/base/object.h"
#include "Microkernel/include/kernel/thread.h"

#include <cstddef>
#include <functional>

namespace Microkernel {

    class SchedulerJob {

    public:

        bool Cyclic = false;
        std::function<void()> Callback;

        SchedulerJob() = default;
        virtual ~SchedulerJob() = default;

    };

    class Scheduler : public Object {

    public:

        static constexpr size_t MaximumThreads = 16;
        bool AddThread(Thread& Thread);
        void RemoveThread(Thread& Thread);
        void AddJob(SchedulerJob& Job);
        void RemoveJob(SchedulerJob& Job);

        void Run();
        void Stop();
        void Yield();
        void Preempt();
        uint32_t* Switch(uint32_t* StackPointer);
        Thread* GetCurrentThread() const;
        bool IsRunning() const;

        Scheduler() = default;
        virtual ~Scheduler() = default;

    private:

        static void Idle();
        Thread* SelectNext();
        Thread* Threads[MaximumThreads] = {};
        size_t ThreadCount = 0;
        size_t CurrentIndex = 0;
        Thread* CurrentThread = nullptr;
        Thread IdleThread;
        bool Running = false;

    };

};
