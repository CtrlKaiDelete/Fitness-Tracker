#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <sys/types.h>

namespace Microkernel {

    uintptr_t WaitSyscall::Execute(uintptr_t Arg0, uintptr_t, uintptr_t) {

        Process* Current = this->Kernel->Processes.GetCurrent();
        if(Current == nullptr) {

            return this->Fail(ECHILD);

        };

        auto Status = reinterpret_cast<int*>(Arg0);
        if(Status != nullptr && !this->ValidateWrite(Status, sizeof(*Status))) {

            return this->Fail(EFAULT);

        };

        Process* Child = this->Kernel->Processes.FindZombieChild(*Current);
        if(Child == nullptr) {

            Thread* CurrentThread = this->Kernel->Scheduler.GetCurrentThread();
            if(CurrentThread != nullptr) {

                Current->GetChildWaitQueue().Wait(*CurrentThread);
                this->Kernel->Scheduler.Yield();

            };

            return this->Fail(EAGAIN);

        };

        uint32_t Identifier = Child->GetIdentifier();
        if(Status != nullptr) {

            *Status = (Child->GetExitStatus() & 0xFF) << 8;

        };

        this->Kernel->Processes.Reap(*Child);
        return Identifier;

    };

};

extern "C" pid_t _wait(int* Status) {

    return static_cast<pid_t>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Wait,
        reinterpret_cast<uintptr_t>(Status)
    ));

};
