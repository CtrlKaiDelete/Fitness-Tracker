#pragma once

#include "Microkernel/include/base/object.h"
#include "Microkernel/include/kernel/executable.h"
#include "Microkernel/include/kernel/resource.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    enum class FileType : uint8_t {

        None,
        Regular,
        Directory,
        SymbolicLink,
        Executable

    };

    class FileNode : public Object {

    public:

        [[nodiscard]] const char* GetPath() const;
        [[nodiscard]] FileType GetType() const;
        [[nodiscard]] size_t GetSize() const;
        [[nodiscard]] uint16_t GetMode() const;
        [[nodiscard]] uint16_t GetOwner() const;
        [[nodiscard]] ExecutableEntry GetEntry() const;
        size_t ReadData(void* Buffer, size_t Size, size_t Offset = 0) const;

    private:

        static constexpr size_t MaximumPathLength = 64;
        static constexpr size_t MaximumFileSize = 512;

        char Path[MaximumPathLength] = {};
        uint8_t Data[MaximumFileSize] = {};
        size_t Size = 0;
        uint16_t Mode = 0;
        uint16_t Owner = 0;
        FileType Type = FileType::None;
        Microkernel::ExecutableEntry EntryPoint = nullptr;
        bool Used = false;

        friend class FileSystem;
        friend class OpenFile;

    };

    class OpenFile : public Resource {

    public:

        OpenFile();

        ptrdiff_t Read(void* Buffer, size_t Size, int& Error);
        ptrdiff_t Write(const void* Buffer, size_t Size, int& Error);
        ptrdiff_t Seek(ptrdiff_t Offset, int Whence, int& Error);

        [[nodiscard]] FileNode* GetNode() const;
        [[nodiscard]] size_t GetOffset() const;
        [[nodiscard]] int GetFlags() const;

    private:

        void Open(FileNode& Node, int Flags);
        void Reset();

        FileNode* Node = nullptr;
        size_t Offset = 0;
        int Flags = 0;

        friend class FileSystem;

    };

    class FileSystem : public Object {

    public:

        static constexpr size_t MaximumNodes = 32;
        static constexpr size_t MaximumOpenFiles = 32;

        bool Initialize();
        bool CreateDirectory(const char* Path, uint16_t Mode = 0755);
        bool CreateFile(const char* Path, const void* Data = nullptr, size_t Size = 0, uint16_t Mode = 0644);
        bool CreateExecutable(const char* Path, ExecutableEntry Entry, uint16_t Mode = 0755);
        bool CreateSymbolicLink(const char* Target, const char* Path);
        bool Remove(const char* Path, int& Error);
        bool ChangeOwner(const char* Path, uint16_t Owner, int& Error);

        FileNode* Resolve(const char* Path, bool FollowSymbolicLinks = true);
        const FileNode* Resolve(const char* Path, bool FollowSymbolicLinks = true) const;
        OpenFile* Open(const char* Path, int Flags, uint16_t Mode, int& Error);

    private:

        static bool NormalizePath(const char* Path, char* Result, size_t Capacity);
        static bool CopyString(char* Destination, size_t Capacity, const char* Source);
        FileNode* CreateNode(const char* Path, FileType Type, uint16_t Mode);
        FileNode* ResolveNormalized(const char* Path, bool FollowSymbolicLinks, uint8_t Depth);
        const FileNode* ResolveNormalized(const char* Path, bool FollowSymbolicLinks, uint8_t Depth) const;
        bool ParentExists(const char* Path) const;
        bool IsOpen(const FileNode& Node) const;

        FileNode Nodes[MaximumNodes] = {};
        OpenFile OpenFiles[MaximumOpenFiles] = {};
        bool Initialized = false;

    };

};
