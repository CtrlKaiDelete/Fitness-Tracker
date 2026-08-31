#include "Microkernel/include/syscalls/unix/syscalls.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cerrno>
#include <sys/stat.h>

namespace Microkernel {

    uintptr_t StatSyscall::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t) {

        char Path[64] = {};
        auto Status = reinterpret_cast<struct stat*>(Arg1);
        if(!this->CopyUserString(reinterpret_cast<const char*>(Arg0), Path, sizeof(Path))) {

            return this->Fail(EFAULT);

        };

        if(!this->ValidateWrite(Status, sizeof(*Status))) {

            return this->Fail(EFAULT);

        };

        const FileNode* Node = this->Kernel->Filesystem.Resolve(Path, true);
        if(Node == nullptr) {

            return this->Fail(ENOENT);

        };

        *Status = {};
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

extern "C" int _stat(const char* Path, struct stat* Status) {

    return static_cast<int>(Microkernel::InvokeUnixSyscall(
        Microkernel::UnixSyscallNumber::Stat,
        reinterpret_cast<uintptr_t>(Path),
        reinterpret_cast<uintptr_t>(Status)
    ));

};
