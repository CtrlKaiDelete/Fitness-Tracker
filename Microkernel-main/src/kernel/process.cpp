#include "Microkernel/include/kernel/process.h"
#include "Microkernel/include/kernel/kernel.h"
#include "Microkernel/include/syscalls/unix/syscalls.h"

#include <new>

extern "C" [[noreturn]] void microkernel_process_exit(int Status);

namespace Microkernel {

    extern "C" uint8_t __kernel_flash_start;
    extern "C" uint8_t __kernel_flash_end;

    Process::Process() {

    };

    Process::~Process() {

        this->Terminate();

    };

    bool Process::Initialize() {

        if(this->State != ProcessState::Created) {

            return false;

        };

        this->StackPage = this->AllocateMemory(
            PagePermission::Read |
            PagePermission::Write |
            PagePermission::User,
            true
        );

        if(this->StackPage == nullptr) {

            return false;

        };

        this->State = ProcessState::Ready;
        return true;

    };

    bool Process::Activate() {

        if(this->State != ProcessState::Ready && this->State != ProcessState::Running) {

            return false;

        };

        if(!this->Kernel->MemoryManager.ActivateAddressSpace(this->ProcessAddressSpace)) {

            return false;

        };

        if(!this->Kernel->MemoryManager.MakeResident(this->ProcessAddressSpace, this->StackPage->GetAddress())) {

            this->Kernel->MemoryManager.DeactivateAddressSpace();
            return false;

        };

        this->State = ProcessState::Running;
        return true;

    };

    bool Process::Start(ExecutableEntry Entry, const ExecutableArguments& Arguments) {

        void* StackTop = nullptr;
        uintptr_t ArgumentVector = 0;
        uintptr_t EnvironmentVector = 0;
        if(this->State != ProcessState::Ready ||
            !this->BuildArgumentStack(Arguments, StackTop, ArgumentVector, EnvironmentVector) ||
            !this->MainThread.InitializeUser(
                reinterpret_cast<uintptr_t>(Entry),
                reinterpret_cast<uintptr_t>(&microkernel_process_exit),
                StackTop,
                *this,
                Arguments.ArgumentCount,
                ArgumentVector,
                EnvironmentVector
            )) {

            return false;

        };

        return this->Kernel->Scheduler.AddThread(this->MainThread);

    };

    bool Process::Execute(ExecutableEntry Entry, const ExecutableArguments& Arguments, ExceptionFrame*& Frame) {

        if(Entry == nullptr || Frame == nullptr || this->State != ProcessState::Running || this->Kernel == nullptr) {

            return false;

        };

        bool ExtendedFrame = (this->Kernel->SyscallManager.GetCurrentExceptionReturn() & 16u) == 0;

        Page* NewStack = this->Kernel->MemoryManager.AllocatePage();
        if(NewStack == nullptr) {

            return false;

        };

        this->Kernel->MemoryManager.ReleaseAddressSpace(this->ProcessAddressSpace);
        for(auto& OwnedPage : this->OwnedPages) {

            OwnedPage = nullptr;

        };

        this->OwnedPageCount = 0;
        this->StackPage = nullptr;

        if(!this->Kernel->MemoryManager.MapPage(
            this->ProcessAddressSpace,
            *NewStack,
            PagePermission::Read | PagePermission::Write | PagePermission::User,
            true
        )) {

            this->Kernel->MemoryManager.FreePage(*NewStack);
            this->State = ProcessState::Terminated;
            return false;

        };

        this->OwnedPages[this->OwnedPageCount++] = NewStack;
        this->StackPage = NewStack;
        this->State = ProcessState::Ready;
        if(!this->Activate()) {

            this->State = ProcessState::Terminated;
            return false;

        };

        void* StackTop = nullptr;
        uintptr_t ArgumentVector = 0;
        uintptr_t EnvironmentVector = 0;
        if(!this->BuildArgumentStack(Arguments, StackTop, ArgumentVector, EnvironmentVector)) {

            this->State = ProcessState::Terminated;
            return false;

        };

        size_t FrameSize = sizeof(ExceptionFrame) + (ExtendedFrame ? 72 : 0);
        auto NewFrame = reinterpret_cast<ExceptionFrame*>(static_cast<uint8_t*>(StackTop) - FrameSize);
        *NewFrame = {};
        NewFrame->R0 = Arguments.ArgumentCount;
        NewFrame->R1 = ArgumentVector;
        NewFrame->R2 = EnvironmentVector;
        NewFrame->LC = reinterpret_cast<uintptr_t>(&microkernel_process_exit);
        NewFrame->PC = reinterpret_cast<uintptr_t>(Entry);
        NewFrame->XPSR = 0x01000000;

        Frame = NewFrame;
        uintptr_t NewStackPointer = reinterpret_cast<uintptr_t>(NewFrame);
        if(ExtendedFrame) {

            auto FloatingPointFrame = reinterpret_cast<uint32_t*>(NewFrame + 1);
            for(size_t Word = 0; Word < 18; Word++) {

                FloatingPointFrame[Word] = 0;

            };

        };

        __asm__ volatile("msr psp, %0\nisb" : : "r"(NewStackPointer) : "memory");
        return true;

    };

    bool Process::BuildArgumentStack(
        const ExecutableArguments& Arguments,
        void*& StackTop,
        uintptr_t& ArgumentVector,
        uintptr_t& EnvironmentVector
    ) {

        if(this->StackPage == nullptr ||
            Arguments.ArgumentCount > ExecutableArguments::MaximumArguments ||
            Arguments.EnvironmentCount > ExecutableArguments::MaximumEnvironment) {

            return false;

        };

        auto Bottom = static_cast<uint8_t*>(this->StackPage->GetAddress());
        uint8_t* Cursor = Bottom + PageSize;
        uintptr_t ArgumentAddresses[ExecutableArguments::MaximumArguments] = {};
        uintptr_t EnvironmentAddresses[ExecutableArguments::MaximumEnvironment] = {};

        for(size_t Reverse = Arguments.EnvironmentCount; Reverse > 0; Reverse--) {

            size_t Index = Reverse - 1;
            size_t Length = 0;
            while(Length < ExecutableArguments::MaximumStringLength &&
                Arguments.Environment[Index][Length] != '\0') {

                Length++;

            };

            if(Length == ExecutableArguments::MaximumStringLength ||
                static_cast<size_t>(Cursor - Bottom) < Length + 1) {

                return false;

            };

            Cursor -= Length + 1;
            for(size_t Character = 0; Character <= Length; Character++) {

                Cursor[Character] = static_cast<uint8_t>(Arguments.Environment[Index][Character]);

            };

            EnvironmentAddresses[Index] = reinterpret_cast<uintptr_t>(Cursor);

        };

        for(size_t Reverse = Arguments.ArgumentCount; Reverse > 0; Reverse--) {

            size_t Index = Reverse - 1;
            size_t Length = 0;
            while(Length < ExecutableArguments::MaximumStringLength && Arguments.Arguments[Index][Length] != '\0') {

                Length++;

            };

            if(Length == ExecutableArguments::MaximumStringLength ||
                static_cast<size_t>(Cursor - Bottom) < Length + 1) {

                return false;

            };

            Cursor -= Length + 1;
            for(size_t Character = 0; Character <= Length; Character++) {

                Cursor[Character] = static_cast<uint8_t>(Arguments.Arguments[Index][Character]);

            };

            ArgumentAddresses[Index] = reinterpret_cast<uintptr_t>(Cursor);

        };

        Cursor = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(Cursor) & ~static_cast<uintptr_t>(7));
        size_t EnvironmentBytes = (Arguments.EnvironmentCount + 1) * sizeof(uintptr_t);
        size_t ArgumentBytes = (Arguments.ArgumentCount + 1) * sizeof(uintptr_t);
        size_t EnvironmentStorage = (EnvironmentBytes + 7) & ~static_cast<size_t>(7);
        size_t ArgumentStorage = (ArgumentBytes + 7) & ~static_cast<size_t>(7);
        constexpr size_t ContextBytes = (18 + 8) * sizeof(uint32_t);
        if(static_cast<size_t>(Cursor - Bottom) < EnvironmentStorage + ArgumentStorage + ContextBytes) {

            return false;

        };

        Cursor -= EnvironmentStorage;
        auto EnvironmentPointers = reinterpret_cast<uintptr_t*>(Cursor);
        for(size_t Index = 0; Index < Arguments.EnvironmentCount; Index++) {

            EnvironmentPointers[Index] = EnvironmentAddresses[Index];

        };

        EnvironmentPointers[Arguments.EnvironmentCount] = 0;
        EnvironmentVector = reinterpret_cast<uintptr_t>(EnvironmentPointers);

        Cursor -= ArgumentStorage;
        auto ArgumentPointers = reinterpret_cast<uintptr_t*>(Cursor);
        for(size_t Index = 0; Index < Arguments.ArgumentCount; Index++) {

            ArgumentPointers[Index] = ArgumentAddresses[Index];

        };

        ArgumentPointers[Arguments.ArgumentCount] = 0;
        ArgumentVector = reinterpret_cast<uintptr_t>(ArgumentPointers);
        StackTop = Cursor;
        return true;

    };

    bool Process::Fork(Process& Child, Thread& ParentThread) {

        if(this->State == ProcessState::Created || this->State == ProcessState::Terminated || Child.State != ProcessState::Created) {

            return false;

        };

        if(!this->Kernel->MemoryManager.CloneAddressSpace(this->ProcessAddressSpace, Child.ProcessAddressSpace)) {

            return false;

        };

        Child.Parent = this;
        Page* ChildStack = Child.AllocateMemory(
            PagePermission::Read |
            PagePermission::Write |
            PagePermission::User,
            true
        );
        if(ChildStack == nullptr) {

            this->Kernel->MemoryManager.ReleaseAddressSpace(Child.ProcessAddressSpace);
            return false;

        };

        this->Kernel->MemoryManager.UnmapPage(Child.ProcessAddressSpace, this->StackPage->GetAddress());
        Child.StackPage = ChildStack;
        Child.State = ProcessState::Ready;
        if(!this->Handles.CloneTo(Child.Handles)) {

            this->Kernel->MemoryManager.ReleaseAddressSpace(Child.ProcessAddressSpace);
            Child.State = ProcessState::Created;
            return false;

        };

        if(!Child.MainThread.Clone(
            ParentThread,
            &Child,
            this->Kernel->SyscallManager.GetCurrentFrame(),
            this->Kernel->SyscallManager.GetCurrentRegisters(),
            this->StackPage->GetAddress(),
            Child.StackPage->GetAddress(),
            PageSize
        ) || !this->Kernel->Scheduler.AddThread(Child.MainThread)) {

            Child.Handles.CloseAll();
            this->Kernel->MemoryManager.ReleaseAddressSpace(Child.ProcessAddressSpace);
            Child.State = ProcessState::Created;
            return false;

        };

        return true;

    };

    void Process::Exit(int Status) {

        if(this->State == ProcessState::Zombie || this->State == ProcessState::Terminated) {

            return;

        };

        this->ExitStatus = Status;
        if(this->Kernel != nullptr) {

            this->Kernel->IPC.RemoveServices(this->Identifier);

        };

        this->Handles.CloseAll();
        this->MainThread.Terminate();
        this->State = ProcessState::Zombie;
        if(this->Parent != nullptr) {

            this->Parent->ChildWaitQueue.WakeAll();

        };

    };

    void Process::Terminate() {

        if(this->State == ProcessState::Terminated) {

            return;

        };

        if(this->Kernel != nullptr && this->Kernel->MemoryManager.GetCurrentAddressSpace() == &this->ProcessAddressSpace) {

            this->Kernel->MemoryManager.DeactivateAddressSpace();

        };

        if(this->Kernel != nullptr) {

            this->Kernel->IPC.RemoveServices(this->Identifier);
            this->Kernel->MemoryManager.ReleaseAddressSpace(this->ProcessAddressSpace);

        };

        this->Handles.CloseAll();
        this->MainThread.Terminate();
        for(auto& OwnedPage : this->OwnedPages) {

            OwnedPage = nullptr;

        };

        this->OwnedPageCount = 0;
        this->StackPage = nullptr;
        this->State = ProcessState::Terminated;

    };

    Page* Process::AllocateMemory(PagePermission Permissions, bool Pinned) {

        if(this->OwnedPageCount >= PageCount || this->Kernel == nullptr) {

            return nullptr;

        };

        Page* NewPage = this->Kernel->MemoryManager.AllocatePage();
        if(NewPage == nullptr) {

            return nullptr;

        };

        if(!this->Kernel->MemoryManager.MapPage(this->ProcessAddressSpace, *NewPage, Permissions, Pinned)) {

            this->Kernel->MemoryManager.FreePage(*NewPage);
            return nullptr;

        };

        this->OwnedPages[this->OwnedPageCount++] = NewPage;
        return NewPage;

    };

    bool Process::ReleaseMemory(Page& Page) {

        if(&Page == this->StackPage || this->Kernel == nullptr) {

            return false;

        };

        for(size_t Index = 0; Index < this->OwnedPageCount; Index++) {

            if(this->OwnedPages[Index] != &Page) {

                continue;

            };

            if(!this->Kernel->MemoryManager.UnmapPage(this->ProcessAddressSpace, Page.GetAddress())) {

                return false;

            };

            if(!this->Kernel->MemoryManager.FreePage(Page)) {

                return false;

            };

            this->OwnedPages[Index] = this->OwnedPages[this->OwnedPageCount - 1];
            this->OwnedPages[this->OwnedPageCount - 1] = nullptr;
            this->OwnedPageCount--;
            return true;

        };

        return false;

    };

    uint32_t Process::GetIdentifier() const {

        return this->Identifier;

    };

    ProcessState Process::GetState() const {

        return this->State;

    };

    AddressSpace& Process::GetAddressSpace() {

        return this->ProcessAddressSpace;

    };

    void* Process::GetStackTop() const {

        if(this->StackPage == nullptr) {

            return nullptr;

        };

        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this->StackPage->GetAddress()) + PageSize);

    };

    bool Process::ValidatePointer(const void* Pointer, size_t Size, PagePermission Permission) const {

        if(Size == 0) {

            return true;

        };

        uintptr_t Start = reinterpret_cast<uintptr_t>(Pointer);
        if(Start == 0 || Size - 1 > UINTPTR_MAX - Start) {

            return false;

        };

        uintptr_t End = Start + Size - 1;
        uintptr_t FlashStart = reinterpret_cast<uintptr_t>(&__kernel_flash_start);
        uintptr_t FlashEnd = reinterpret_cast<uintptr_t>(&__kernel_flash_end);
        if(Permission == PagePermission::Read && Start >= FlashStart && End < FlashEnd) {

            return true;

        };

        for(uintptr_t Address = Start; Address <= End;) {

            const PageMapping* Mapping = this->ProcessAddressSpace.FindMapping(Address);
            if(
                Mapping == nullptr ||
                !HasPermission(Mapping->GetPermissions(), PagePermission::User) ||
                !HasPermission(Mapping->GetPermissions(), Permission)
            ) {

                return false;

            };

            uintptr_t Next = (Address & ~(PageSize - 1)) + PageSize;
            if(Next == 0 || Next > End) {

                break;

            };

            Address = Next;

        };

        return true;

    };

    Process* Process::GetParent() const {

        return this->Parent;

    };

    int Process::GetExitStatus() const {

        return this->ExitStatus;

    };

    Thread& Process::GetMainThread() {

        return this->MainThread;

    };

    HandleTable& Process::GetHandles() {

        return this->Handles;

    };

    WaitQueue& Process::GetChildWaitQueue() {

        return this->ChildWaitQueue;

    };

    Process* ProcessTable::Create(Process* Parent) {

        for(size_t Index = 0; Index < MaximumProcesses; Index++) {

            if(this->Used[Index]) {

                continue;

            };

            this->Used[Index] = true;
            this->Processes[Index].Identifier = Process::NextIdentifier++;
            this->Processes[Index].Parent = Parent;
            if(!this->Processes[Index].Initialize()) {

                this->Used[Index] = false;
                return nullptr;

            };

            return &this->Processes[Index];

        };

        return nullptr;

    };

    Process* ProcessTable::ForkCurrent() {

        Process* Parent = this->GetCurrent();
        Thread* ParentThread = this->Kernel->Scheduler.GetCurrentThread();
        if(Parent == nullptr || ParentThread == nullptr) {

            return nullptr;

        };

        for(size_t Index = 0; Index < MaximumProcesses; Index++) {

            if(this->Used[Index]) {

                continue;

            };

            this->Used[Index] = true;
            this->Processes[Index].Identifier = Process::NextIdentifier++;
            if(!Parent->Fork(this->Processes[Index], *ParentThread)) {

                this->Used[Index] = false;
                return nullptr;

            };

            return &this->Processes[Index];

        };

        return nullptr;

    };

    Process* ProcessTable::Find(uint32_t Identifier) {

        for(size_t Index = 0; Index < MaximumProcesses; Index++) {

            if(this->Used[Index] && this->Processes[Index].GetIdentifier() == Identifier) {

                return &this->Processes[Index];

            };

        };

        return nullptr;

    };

    Process* ProcessTable::GetCurrent() const {

        Thread* Current = this->Kernel->Scheduler.GetCurrentThread();
        return Current == nullptr ? nullptr : Current->GetProcess();

    };

    Process* ProcessTable::FindZombieChild(Process& Parent) {

        for(size_t Index = 0; Index < MaximumProcesses; Index++) {

            if(this->Used[Index] && this->Processes[Index].Parent == &Parent && this->Processes[Index].State == ProcessState::Zombie) {

                return &this->Processes[Index];

            };

        };

        return nullptr;

    };

    bool ProcessTable::Reap(Process& Process) {

        for(size_t Index = 0; Index < MaximumProcesses; Index++) {

            if(!this->Used[Index] || &this->Processes[Index] != &Process || Process.State != ProcessState::Zombie) {

                continue;

            };

            Process.Terminate();
            this->Used[Index] = false;
            Process.~Process();
            new (&Process) Microkernel::Process();
            return true;

        };

        return false;

    };

};

extern "C" [[noreturn]] void microkernel_process_exit(int Status) {

    Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Exit,
        static_cast<uintptr_t>(Status)
    );

    while(true) {

        __asm__ volatile("wfi");

    };

};
