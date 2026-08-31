#include "Microkernel/include/kernel/filesystem.h"

#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>

namespace Microkernel {

    const char* FileNode::GetPath() const {

        return this->Path;

    };

    FileType FileNode::GetType() const {

        return this->Type;

    };

    size_t FileNode::GetSize() const {

        return this->Size;

    };

    uint16_t FileNode::GetMode() const {

        return this->Mode;

    };

    uint16_t FileNode::GetOwner() const {

        return this->Owner;

    };

    ExecutableEntry FileNode::GetEntry() const {

        return this->EntryPoint;

    };

    size_t FileNode::ReadData(void* Buffer, size_t Size, size_t Offset) const {

        if(Buffer == nullptr || Offset >= this->Size) {

            return 0;

        };

        size_t Remaining = this->Size - Offset;
        size_t Count = Size < Remaining ? Size : Remaining;
        auto Destination = static_cast<uint8_t*>(Buffer);
        for(size_t Index = 0; Index < Count; Index++) {

            Destination[Index] = this->Data[Offset + Index];

        };

        return Count;

    };

    OpenFile::OpenFile() : Resource(ResourceType::File) {

    };

    ptrdiff_t OpenFile::Read(void* Buffer, size_t Size, int& Error) {

        Error = 0;
        if(this->Node == nullptr) {

            Error = EBADF;
            return -1;

        };

        if((this->Flags & O_ACCMODE) == O_WRONLY) {

            Error = EBADF;
            return -1;

        };

        if(this->Node->Type == FileType::Directory) {

            Error = EISDIR;
            return -1;

        };

        if(Size == 0) {

            return 0;

        };

        if(Buffer == nullptr) {

            Error = EFAULT;
            return -1;

        };

        if(this->Offset >= this->Node->Size) {

            return 0;

        };

        size_t Remaining = this->Node->Size - this->Offset;
        size_t Count = Size < Remaining ? Size : Remaining;
        auto Destination = static_cast<uint8_t*>(Buffer);
        for(size_t Index = 0; Index < Count; Index++) {

            Destination[Index] = this->Node->Data[this->Offset + Index];

        };

        this->Offset += Count;
        return static_cast<ptrdiff_t>(Count);

    };

    ptrdiff_t OpenFile::Write(const void* Buffer, size_t Size, int& Error) {

        Error = 0;
        if(this->Node == nullptr) {

            Error = EBADF;
            return -1;

        };

        if((this->Flags & O_ACCMODE) == O_RDONLY) {

            Error = EBADF;
            return -1;

        };

        if(this->Node->Type != FileType::Regular) {

            Error = this->Node->Type == FileType::Directory ? EISDIR : EACCES;
            return -1;

        };

        if(Size == 0) {

            return 0;

        };

        if(Buffer == nullptr) {

            Error = EFAULT;
            return -1;

        };

        if((this->Flags & O_APPEND) != 0) {

            this->Offset = this->Node->Size;

        };

        if(this->Offset >= FileNode::MaximumFileSize && Size != 0) {

            Error = ENOSPC;
            return -1;

        };

        size_t Available = FileNode::MaximumFileSize - this->Offset;
        size_t Count = Size < Available ? Size : Available;
        auto Source = static_cast<const uint8_t*>(Buffer);
        for(size_t Index = 0; Index < Count; Index++) {

            this->Node->Data[this->Offset + Index] = Source[Index];

        };

        this->Offset += Count;
        if(this->Offset > this->Node->Size) {

            this->Node->Size = this->Offset;

        };

        if(Count != Size) {

            Error = ENOSPC;

        };

        return static_cast<ptrdiff_t>(Count);

    };

    ptrdiff_t OpenFile::Seek(ptrdiff_t Offset, int Whence, int& Error) {

        Error = 0;
        if(this->Node == nullptr) {

            Error = EBADF;
            return -1;

        };

        if(this->Node->Type == FileType::Directory) {

            Error = EISDIR;
            return -1;

        };

        ptrdiff_t Base = 0;
        if(Whence == SEEK_CUR) {

            Base = static_cast<ptrdiff_t>(this->Offset);

        } else if(Whence == SEEK_END) {

            Base = static_cast<ptrdiff_t>(this->Node->Size);

        } else if(Whence != SEEK_SET) {

            Error = EINVAL;
            return -1;

        };

        if((Offset < 0 && Base < -Offset) || (Offset > 0 && Base > static_cast<ptrdiff_t>(FileNode::MaximumFileSize) - Offset)) {

            Error = EINVAL;
            return -1;

        };

        ptrdiff_t NewOffset = Base + Offset;
        if(NewOffset < 0 || NewOffset > static_cast<ptrdiff_t>(FileNode::MaximumFileSize)) {

            Error = EINVAL;
            return -1;

        };

        this->Offset = static_cast<size_t>(NewOffset);
        return NewOffset;

    };

    FileNode* OpenFile::GetNode() const {

        return this->Node;

    };

    size_t OpenFile::GetOffset() const {

        return this->Offset;

    };

    int OpenFile::GetFlags() const {

        return this->Flags;

    };

    void OpenFile::Open(FileNode& Node, int Flags) {

        this->Node = &Node;
        this->Flags = Flags;
        this->Offset = (Flags & O_APPEND) != 0 ? Node.Size : 0;

    };

    void OpenFile::Reset() {

        this->Node = nullptr;
        this->Offset = 0;
        this->Flags = 0;

    };

    bool FileSystem::Initialize() {

        if(this->Initialized) {

            return true;

        };

        FileNode* Root = this->CreateNode("/", FileType::Directory, 0755);
        if(Root == nullptr) {

            return false;

        };

        this->Initialized = true;
        return true;

    };

    bool FileSystem::CreateDirectory(const char* Path, uint16_t Mode) {

        return this->CreateNode(Path, FileType::Directory, Mode) != nullptr;

    };

    bool FileSystem::CreateFile(const char* Path, const void* Data, size_t Size, uint16_t Mode) {

        if(Size > FileNode::MaximumFileSize || (Data == nullptr && Size != 0)) {

            return false;

        };

        FileNode* Node = this->CreateNode(Path, FileType::Regular, Mode);
        if(Node == nullptr) {

            return false;

        };

        auto Source = static_cast<const uint8_t*>(Data);
        for(size_t Index = 0; Index < Size; Index++) {

            Node->Data[Index] = Source[Index];

        };

        Node->Size = Size;
        return true;

    };

    bool FileSystem::CreateExecutable(const char* Path, ExecutableEntry Entry, uint16_t Mode) {

        if(Entry == nullptr) {

            return false;

        };

        FileNode* Node = this->CreateNode(Path, FileType::Executable, Mode);
        if(Node == nullptr) {

            return false;

        };

        Node->EntryPoint = Entry;
        return true;

    };

    bool FileSystem::CreateSymbolicLink(const char* Target, const char* Path) {

        char NormalizedTarget[FileNode::MaximumPathLength] = {};
        if(!NormalizePath(Target, NormalizedTarget, sizeof(NormalizedTarget))) {

            return false;

        };

        FileNode* Node = this->CreateNode(Path, FileType::SymbolicLink, 0777);
        if(Node == nullptr) {

            return false;

        };

        size_t Length = 0;
        while(NormalizedTarget[Length] != '\0') {

            Node->Data[Length] = static_cast<uint8_t>(NormalizedTarget[Length]);
            Length++;

        };

        Node->Size = Length;
        return true;

    };

    bool FileSystem::Remove(const char* Path, int& Error) {

        Error = 0;
        char Normalized[FileNode::MaximumPathLength] = {};
        if(!NormalizePath(Path, Normalized, sizeof(Normalized)) || Normalized[1] == '\0') {

            Error = EINVAL;
            return false;

        };

        FileNode* Node = this->ResolveNormalized(Normalized, false, 0);
        if(Node == nullptr) {

            Error = ENOENT;
            return false;

        };

        if(Node->Type == FileType::Directory) {

            size_t Length = 0;
            while(Normalized[Length] != '\0') {

                Length++;

            };

            for(const FileNode& Candidate : this->Nodes) {

                if(!Candidate.Used || &Candidate == Node) {

                    continue;

                };

                bool Child = true;
                for(size_t Index = 0; Index < Length; Index++) {

                    if(Candidate.Path[Index] != Normalized[Index]) {

                        Child = false;
                        break;

                    };

                };

                if(Child && Candidate.Path[Length] == '/') {

                    Error = ENOTEMPTY;
                    return false;

                };

            };

        };

        if(this->IsOpen(*Node)) {

            Error = EBUSY;
            return false;

        };

        *Node = {};
        return true;

    };

    bool FileSystem::ChangeOwner(const char* Path, uint16_t Owner, int& Error) {

        Error = 0;
        FileNode* Node = this->Resolve(Path, false);
        if(Node == nullptr) {

            Error = ENOENT;
            return false;

        };

        Node->Owner = Owner;
        return true;

    };

    FileNode* FileSystem::Resolve(const char* Path, bool FollowSymbolicLinks) {

        char Normalized[FileNode::MaximumPathLength] = {};
        if(!NormalizePath(Path, Normalized, sizeof(Normalized))) {

            return nullptr;

        };

        return this->ResolveNormalized(Normalized, FollowSymbolicLinks, 0);

    };

    const FileNode* FileSystem::Resolve(const char* Path, bool FollowSymbolicLinks) const {

        char Normalized[FileNode::MaximumPathLength] = {};
        if(!NormalizePath(Path, Normalized, sizeof(Normalized))) {

            return nullptr;

        };

        return this->ResolveNormalized(Normalized, FollowSymbolicLinks, 0);

    };

    OpenFile* FileSystem::Open(const char* Path, int Flags, uint16_t Mode, int& Error) {

        Error = 0;
        FileNode* Node = this->Resolve(Path, true);
        if(Node == nullptr && (Flags & O_CREAT) != 0) {

            if(!this->CreateFile(Path, nullptr, 0, Mode)) {

                Error = ENOENT;
                return nullptr;

            };

            Node = this->Resolve(Path, true);

        } else if(Node != nullptr && (Flags & O_CREAT) != 0 && (Flags & O_EXCL) != 0) {

            Error = EEXIST;
            return nullptr;

        };

        if(Node == nullptr) {

            Error = ENOENT;
            return nullptr;

        };

        if(Node->Type == FileType::Directory && (Flags & O_ACCMODE) != O_RDONLY) {

            Error = EISDIR;
            return nullptr;

        };

        if(Node->Type != FileType::Regular && Node->Type != FileType::Directory) {

            Error = EACCES;
            return nullptr;

        };

        for(OpenFile& File : this->OpenFiles) {

            if(File.GetReferenceCount() != 0) {

                continue;

            };

            File.Reset();
            if((Flags & O_TRUNC) != 0 && (Flags & O_ACCMODE) != O_RDONLY && Node->Type == FileType::Regular) {

                Node->Size = 0;

            };

            File.Open(*Node, Flags);
            return &File;

        };

        Error = ENFILE;
        return nullptr;

    };

    bool FileSystem::NormalizePath(const char* Path, char* Result, size_t Capacity) {

        if(Path == nullptr || Result == nullptr || Capacity < 2 || Path[0] != '/') {

            return false;

        };

        size_t Input = 0;
        size_t Output = 0;
        while(Path[Input] != '\0') {

            char Character = Path[Input++];
            if(Character == '/' && Output != 0 && Result[Output - 1] == '/') {

                continue;

            };

            if(Output + 1 >= Capacity) {

                return false;

            };

            Result[Output++] = Character;

        };

        if(Output > 1 && Result[Output - 1] == '/') {

            Output--;

        };

        Result[Output] = '\0';
        if(Output == 0) {

            return false;

        };

        for(size_t Index = 0; Index < Output; Index++) {

            if(Result[Index] != '/') {

                continue;

            };

            size_t Start = Index + 1;
            size_t End = Start;
            while(End < Output && Result[End] != '/') {

                End++;

            };

            if((End - Start == 1 && Result[Start] == '.') ||
                (End - Start == 2 && Result[Start] == '.' && Result[Start + 1] == '.')) {

                return false;

            };

        };

        return true;

    };

    bool FileSystem::CopyString(char* Destination, size_t Capacity, const char* Source) {

        if(Destination == nullptr || Source == nullptr || Capacity == 0) {

            return false;

        };

        size_t Index = 0;
        while(Source[Index] != '\0') {

            if(Index + 1 >= Capacity) {

                return false;

            };

            Destination[Index] = Source[Index];
            Index++;

        };

        Destination[Index] = '\0';
        return true;

    };

    FileNode* FileSystem::CreateNode(const char* Path, FileType Type, uint16_t Mode) {

        char Normalized[FileNode::MaximumPathLength] = {};
        if(Type == FileType::None || !NormalizePath(Path, Normalized, sizeof(Normalized)) ||
            this->ResolveNormalized(Normalized, false, 0) != nullptr || !this->ParentExists(Normalized)) {

            return nullptr;

        };

        for(FileNode& Node : this->Nodes) {

            if(Node.Used) {

                continue;

            };

            Node = {};
            if(!CopyString(Node.Path, sizeof(Node.Path), Normalized)) {

                return nullptr;

            };

            Node.Type = Type;
            Node.Mode = Mode & 0777;
            Node.Used = true;
            return &Node;

        };

        return nullptr;

    };

    FileNode* FileSystem::ResolveNormalized(const char* Path, bool FollowSymbolicLinks, uint8_t Depth) {

        if(Depth > 8) {

            return nullptr;

        };

        for(FileNode& Node : this->Nodes) {

            if(!Node.Used) {

                continue;

            };

            size_t Index = 0;
            while(Node.Path[Index] == Path[Index] && Path[Index] != '\0') {

                Index++;

            };

            if(Node.Path[Index] != '\0' || Path[Index] != '\0') {

                continue;

            };

            if(Node.Type != FileType::SymbolicLink || !FollowSymbolicLinks) {

                return &Node;

            };

            char Target[FileNode::MaximumPathLength] = {};
            for(size_t TargetIndex = 0; TargetIndex < Node.Size; TargetIndex++) {

                Target[TargetIndex] = static_cast<char>(Node.Data[TargetIndex]);

            };

            return this->ResolveNormalized(Target, true, Depth + 1);

        };

        return nullptr;

    };

    const FileNode* FileSystem::ResolveNormalized(const char* Path, bool FollowSymbolicLinks, uint8_t Depth) const {

        return const_cast<FileSystem*>(this)->ResolveNormalized(Path, FollowSymbolicLinks, Depth);

    };

    bool FileSystem::ParentExists(const char* Path) const {

        if(Path[0] == '/' && Path[1] == '\0') {

            return true;

        };

        char Parent[FileNode::MaximumPathLength] = {};
        size_t LastSlash = 0;
        size_t Length = 0;
        while(Path[Length] != '\0') {

            if(Path[Length] == '/') {

                LastSlash = Length;

            };

            Length++;

        };

        if(LastSlash == 0) {

            Parent[0] = '/';
            Parent[1] = '\0';

        } else {

            for(size_t Index = 0; Index < LastSlash; Index++) {

                Parent[Index] = Path[Index];

            };

            Parent[LastSlash] = '\0';

        };

        const FileNode* Node = this->ResolveNormalized(Parent, true, 0);
        return Node != nullptr && Node->Type == FileType::Directory;

    };

    bool FileSystem::IsOpen(const FileNode& Node) const {

        for(const OpenFile& File : this->OpenFiles) {

            if(File.GetReferenceCount() != 0 && File.GetNode() == &Node) {

                return true;

            };

        };

        return false;

    };

};
