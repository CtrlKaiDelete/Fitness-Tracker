#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/filesystem.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>

namespace Microkernel {

    uintptr_t ExecveSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        char Path[64] = {};
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        FileNode* Node = this->Kernel->Filesystem.Resolve(Path);
        if(Node == nullptr) {

            return this->Fail(ENOENT);

        };

        if(Node->GetType() != FileType::Executable || Node->GetEntry() == nullptr) {

            return this->Fail(ENOEXEC);

        };

        if((Node->GetMode() & 0111) == 0) {

            return this->Fail(EACCES);

        };

        ExecutableArguments Arguments = {};
        auto CopyVector = [this](
            uintptr_t Address,
            char Destination[][ExecutableArguments::MaximumStringLength],
            size_t Capacity,
            size_t& Count
        ) -> int {

            Count = 0;
            if(Address == 0) {

                return 0;

            };

            auto Vector = reinterpret_cast<const char* const*>(Address);
            for(size_t Index = 0; Index <= Capacity; Index++) {

                if(!this->ValidateRead(&Vector[Index], sizeof(Vector[Index]))) {

                    return EFAULT;

                };

                const char* Value = Vector[Index];
                if(Value == nullptr) {

                    return 0;

                };

                if(Index == Capacity) {

                    return E2BIG;

                };

                if(!this->CopyUserString(Value, Destination[Index], ExecutableArguments::MaximumStringLength)) {

                    return E2BIG;

                };

                Count++;

            };

            return E2BIG;

        };

        int Error = CopyVector(
            Arg1,
            Arguments.Arguments,
            ExecutableArguments::MaximumArguments,
            Arguments.ArgumentCount
        );
        if(Error != 0) {

            return this->Fail(Error);

        };

        Error = CopyVector(
            Arg2,
            Arguments.Environment,
            ExecutableArguments::MaximumEnvironment,
            Arguments.EnvironmentCount
        );
        if(Error != 0) {

            return this->Fail(Error);

        };

        if(Arguments.ArgumentCount == 0) {

            size_t Index = 0;
            while(Index + 1 < ExecutableArguments::MaximumStringLength && Path[Index] != '\0') {

                Arguments.Arguments[0][Index] = Path[Index];
                Index++;

            };

            Arguments.Arguments[0][Index] = '\0';
            Arguments.ArgumentCount = 1;

        };

        size_t StackBytes = (18 + 8) * sizeof(uint32_t);
        size_t ArgumentBytes = (Arguments.ArgumentCount + 1) * sizeof(uintptr_t);
        size_t EnvironmentBytes = (Arguments.EnvironmentCount + 1) * sizeof(uintptr_t);
        StackBytes += (ArgumentBytes + 7) & ~static_cast<size_t>(7);
        StackBytes += (EnvironmentBytes + 7) & ~static_cast<size_t>(7);
        for(size_t Index = 0; Index < Arguments.ArgumentCount; Index++) {

            size_t Length = 0;
            while(Arguments.Arguments[Index][Length] != '\0') {

                Length++;

            };

            StackBytes += Length + 1;

        };

        for(size_t Index = 0; Index < Arguments.EnvironmentCount; Index++) {

            size_t Length = 0;
            while(Arguments.Environment[Index][Length] != '\0') {

                Length++;

            };

            StackBytes += Length + 1;

        };

        if(StackBytes + 8 > PageSize) {

            return this->Fail(E2BIG);

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        ExceptionFrame* Frame = const_cast<ExceptionFrame*>(this->Kernel->SyscallManager.GetCurrentFrame());
        if(Current == nullptr || Frame == nullptr) {

            return this->Fail(EPERM);

        };

        if(!Current->Execute(Node->GetEntry(), Arguments, Frame)) {

            return this->Fail(ENOMEM);

        };

        if(!this->Kernel->SyscallManager.ReplaceCurrentExceptionFrame(Frame)) {

            this->Kernel->Panic("EXECVE FRAME");

        };

        return 0;

    };

};

extern "C" int _execve(const char* Path, char* const Arguments[], char* const Environment[]) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Execve,
        reinterpret_cast<uintptr_t>(Path),
        reinterpret_cast<uintptr_t>(Arguments),
        reinterpret_cast<uintptr_t>(Environment)
    ));

};
