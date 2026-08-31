#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <sys/stat.h>

namespace Microkernel {

    uintptr_t FstatSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        int FileDescriptor = static_cast<int>(Arg0);
        auto Status = reinterpret_cast<struct stat*>(Arg1);
        if(!this->ValidateWrite(Status, sizeof(*Status))) {

            return this->Fail(EFAULT);

        };

        *Status = {};
        if(this->IsStandardDescriptor(FileDescriptor)) {

            Status->st_mode = S_IFCHR | 0666;
            Status->st_blksize = 1;
            return 0;

        };

        Process* Current = this->Kernel->Processes.GetCurrent();
        auto File = Current == nullptr ? nullptr : static_cast<OpenFile*>(Current->GetHandles().Get(
            FileDescriptor,
            Capability::None,
            ResourceType::File
        ));
        if(File == nullptr || File->GetNode() == nullptr) {

            return this->Fail(EBADF);

        };

        FileNode* Node = File->GetNode();
        Status->st_mode = Node->GetMode();
        if(Node->GetType() == FileType::Directory) {

            Status->st_mode |= S_IFDIR;

        } else if(Node->GetType() == FileType::SymbolicLink) {

            Status->st_mode |= S_IFLNK;

        } else {

            Status->st_mode |= S_IFREG;

        };

        Status->st_size = static_cast<off_t>(Node->GetSize());
        Status->st_uid = Node->GetOwner();
        Status->st_nlink = 1;
        Status->st_blksize = 1;
        return 0;

    };

};

extern "C" int _fstat(int FileDescriptor, struct stat* Status) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Fstat,
        static_cast<uintptr_t>(FileDescriptor),
        reinterpret_cast<uintptr_t>(Status)
    ));

};
