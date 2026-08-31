// Created by Julien Rodot on 28/8/26. Licensed under MIT

#include "Microkernel/include/kernel/kernel.h"
#include "Microkernel/include/interrupts/displayinterrupt.h"
#include "Microkernel/include/interrupts/timerinterrupt.h"
#include "Microkernel/include/kernel/panic.h"
#include "Microkernel/include/kernel/timer.h"
#include "Microkernel/include/syscalls/ipc.h"
#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/syscalls/watch.h"
#include "Microkernel/include/syscalls/writepixel.h"
#include "Microwatch/Applications/Clock.app/Headers/Clock.h"
#include "Microwatch/Applications/Compass.app/Headers/Compass.h"
#include "Microwatch/Applications/Steps.app/Headers/Steps.h"
#include "Microwatch/Applications/Stopwatch.app/Headers/Stopwatch.h"
#include "Microwatch/Applications/Timer.app/Headers/Timer.h"
#include "Microwatch/sbin/init/Headers/Init.h"
#include "Microwatch/System/Library/CoreServices/SpringBoard.app/Headers/SpringBoard.h"
#include "Microwatch/usr/libexec/bluetoothd/Headers/BluetoothDaemon.h"
#include "Microwatch/usr/libexec/motiond/Headers/MotionDaemon.h"
#include "Microwatch/usr/libexec/notifyd/Headers/NotificationDaemon.h"
#include "Microwatch/usr/libexec/powerd/Headers/PowerDaemon.h"
#include "Microwatch/usr/libexec/timed/Headers/TimeDaemon.h"

#include <cstddef>
#include <new>

namespace Microkernel {

    Kernel* Kernel::New() {

        alignas(Kernel) static std::byte Storage[sizeof(Kernel)];
        static Kernel* Instance = nullptr;
        if(Instance == nullptr) {

            Instance = new (Storage) Kernel();
            Object::Kernel = Instance;

        };

        return Instance;

    };

};

extern "C" void KernelMain() {

    using namespace Microkernel;

    auto Kernel = Kernel::New();
    if(!Kernel->MemoryManager.Initialize()) {

        /*
         * Thats not good...
         */

        Kernel->Panic("MPU NOT AVAILABLE");

    };

    if(!Kernel->DriverWatchdog.Initialize()) {

        Kernel->Panic("WATCHDOG INIT");

    };

    if(!Kernel->Filesystem.Initialize() ||
        !Kernel->Filesystem.CreateDirectory("/sbin") ||
        !Kernel->Filesystem.CreateDirectory("/tmp", 0777) ||
        !Kernel->Filesystem.CreateDirectory("/usr") ||
        !Kernel->Filesystem.CreateDirectory("/usr/libexec") ||
        !Kernel->Filesystem.CreateDirectory("/var") ||
        !Kernel->Filesystem.CreateDirectory("/var/run") ||
        !Kernel->Filesystem.CreateDirectory("/Library") ||
        !Kernel->Filesystem.CreateDirectory("/Library/Application Support") ||
        !Kernel->Filesystem.CreateDirectory("/System") ||
        !Kernel->Filesystem.CreateDirectory("/System/Library") ||
        !Kernel->Filesystem.CreateDirectory("/System/Library/CoreServices") ||
        !Kernel->Filesystem.CreateDirectory("/System/Library/Frameworks") ||
        !Kernel->Filesystem.CreateDirectory("/System/Library/PrivateFrameworks") ||
        !Kernel->Filesystem.CreateDirectory("/System/Library/LaunchDaemons") ||
        !Kernel->Filesystem.CreateDirectory("/System/Library/CoreServices/SpringBoard.app") ||
        !Kernel->Filesystem.CreateDirectory("/Applications") ||
        !Kernel->Filesystem.CreateExecutable(Microwatch::Init::ExecutablePath, &Microwatch::InitMain) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::MotionDaemon::ExecutablePath,
            &Microwatch::MotionDaemonMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::PowerDaemon::ExecutablePath,
            &Microwatch::PowerDaemonMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::TimeDaemon::ExecutablePath,
            &Microwatch::TimeDaemonMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::NotificationDaemon::ExecutablePath,
            &Microwatch::NotificationDaemonMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::BluetoothDaemon::ExecutablePath,
            &Microwatch::BluetoothDaemonMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::Springboard::ExecutablePath,
            &Microwatch::SpringboardMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(Microwatch::ClockApplication::ExecutablePath, &Microwatch::ClockMain) ||
        !Kernel->Filesystem.CreateExecutable(Microwatch::StepsApplication::ExecutablePath, &Microwatch::StepsMain) ||
        !Kernel->Filesystem.CreateExecutable(
            Microwatch::StopwatchApplication::ExecutablePath,
            &Microwatch::StopwatchMain
        ) ||
        !Kernel->Filesystem.CreateExecutable(Microwatch::TimerApplication::ExecutablePath, &Microwatch::TimerMain) ||
        !Kernel->Filesystem.CreateExecutable(Microwatch::CompassApplication::ExecutablePath, &Microwatch::CompassMain)) {

        Kernel->Panic("FILESYSTEM INIT");

    };

    Kernel->SyscallManager.RegisterSyscall<ChownSyscall>();
    Kernel->SyscallManager.RegisterSyscall<CloseSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ExecveSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ExitSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ForkSyscall>();
    Kernel->SyscallManager.RegisterSyscall<FstatSyscall>();
    Kernel->SyscallManager.RegisterSyscall<GetEntropySyscall>();
    Kernel->SyscallManager.RegisterSyscall<GetPidSyscall>();
    Kernel->SyscallManager.RegisterSyscall<GetTimeOfDaySyscall>();
    Kernel->SyscallManager.RegisterSyscall<IsATtySyscall>();
    Kernel->SyscallManager.RegisterSyscall<KillSyscall>();
    Kernel->SyscallManager.RegisterSyscall<LinkSyscall>();
    Kernel->SyscallManager.RegisterSyscall<LseekSyscall>();
    Kernel->SyscallManager.RegisterSyscall<OpenSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ReadSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ReadLinkSyscall>();
    Kernel->SyscallManager.RegisterSyscall<SbrkSyscall>();
    Kernel->SyscallManager.RegisterSyscall<StatSyscall>();
    Kernel->SyscallManager.RegisterSyscall<SymlinkSyscall>();
    Kernel->SyscallManager.RegisterSyscall<TimesSyscall>();
    Kernel->SyscallManager.RegisterSyscall<UnlinkSyscall>();
    Kernel->SyscallManager.RegisterSyscall<WaitSyscall>();
    Kernel->SyscallManager.RegisterSyscall<WriteSyscall>();
    Kernel->SyscallManager.RegisterSyscall<WritePixel>();
    Kernel->SyscallManager.RegisterSyscall<ReadControlsSyscall>();
    Kernel->SyscallManager.RegisterSyscall<PlayToneSyscall>();
    Kernel->SyscallManager.RegisterSyscall<StopToneSyscall>();
    Kernel->SyscallManager.RegisterSyscall<UpdateBluetoothSyscall>();
    Kernel->SyscallManager.RegisterSyscall<GetTicksSyscall>();
    Kernel->SyscallManager.RegisterSyscall<DebugPanicSyscall>();
    Kernel->SyscallManager.RegisterSyscall<YieldSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ReadAccelerationSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ReadMagneticFieldSyscall>();
    Kernel->SyscallManager.RegisterSyscall<CreateEndpointSyscall>();
    Kernel->SyscallManager.RegisterSyscall<IPCSendSyscall>();
    Kernel->SyscallManager.RegisterSyscall<IPCReceiveSyscall>();
    Kernel->SyscallManager.RegisterSyscall<CloseHandleSyscall>();
    Kernel->SyscallManager.RegisterSyscall<RegisterServiceSyscall>();
    Kernel->SyscallManager.RegisterSyscall<ConnectServiceSyscall>();

    Kernel->InterruptManager.RegisterInterrupt<TimerInterrupt>();
    Kernel->InterruptManager.RegisterInterrupt<DisplayInterrupt>();
    Kernel->InterruptTimer.Start();

    FileNode* InitNode = Kernel->Filesystem.Resolve("/sbin/init");
    Process* Init = Kernel->Processes.Create();
    if(InitNode == nullptr || Init == nullptr || Init->GetIdentifier() != 1) {

        Kernel->Panic("INIT PROCESS");

    };

    ExecutableArguments InitArguments = {};
    constexpr char InitPath[] = "/sbin/init";
    for(size_t Index = 0; Index < sizeof(InitPath); Index++) {

        InitArguments.Arguments[0][Index] = InitPath[Index];

    };

    InitArguments.ArgumentCount = 1;
    if(!Init->Start(InitNode->GetEntry(), InitArguments)) {

        Kernel->Panic("INIT START");

    };

    Kernel->Scheduler.Run();

};
