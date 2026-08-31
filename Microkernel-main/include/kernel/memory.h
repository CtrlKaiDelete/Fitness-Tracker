#pragma once

#include "Microkernel/include/base/object.h"
#include "Microkernel/include/libkern/exceptionframe.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    static constexpr size_t PageSize = 1024;
    static constexpr size_t PageCount = 32;
    static constexpr size_t MaximumPageMappings = PageCount;
    static constexpr uint8_t InvalidMPURegion = 0xFF;

    enum class PagePermission : uint8_t {

        None = 0,
        Read = 1u << 0,
        Write = 1u << 1,
        Execute = 1u << 2,
        User = 1u << 3

    };

    constexpr PagePermission operator|(PagePermission Left, PagePermission Right) {

        return static_cast<PagePermission>(static_cast<uint8_t>(Left) | static_cast<uint8_t>(Right));

    };

    constexpr bool HasPermission(PagePermission Permissions, PagePermission Permission) {

        return (static_cast<uint8_t>(Permissions) & static_cast<uint8_t>(Permission)) != 0;

    };

    enum class PageFaultResult : uint8_t {

        Handled,
        InvalidAddress,
        PermissionDenied,
        NoAddressSpace,
        NoMPURegion

    };

    struct PageFaultInfo {

        uintptr_t Address = 0;
        uint32_t FaultStatus = 0;
        PageFaultResult Result = PageFaultResult::InvalidAddress;

    };

    class MemoryManager;
    class AddressSpace;

    class Page {

    public:

        Page() = default;
        virtual ~Page() = default;

        void* GetAddress() const;
        bool IsAllocated() const;

    private:

        uintptr_t Address = 0;
        uint16_t References = 0;
        bool Allocated = false;

        friend class MemoryManager;

    };

    class PageMapping {

    public:

        PageMapping() = default;
        virtual ~PageMapping() = default;

        uintptr_t GetAddress() const;
        PagePermission GetPermissions() const;
        bool IsPresent() const;
        bool IsResident() const;
        bool IsPinned() const;

    private:

        Page* PhysicalPage = nullptr;
        PagePermission Permissions = PagePermission::None;
        uint32_t LastUsed = 0;
        uint8_t MPURegion = InvalidMPURegion;
        bool Present = false;
        bool Pinned = false;

        friend class AddressSpace;
        friend class MemoryManager;

    };

    class AddressSpace : public Object {

    public:

        AddressSpace() = default;
        virtual ~AddressSpace() = default;

        PageMapping* FindMapping(uintptr_t Address);
        const PageMapping* FindMapping(uintptr_t Address) const;
        size_t GetMappingCount() const;

    private:

        PageMapping Mappings[MaximumPageMappings] = {};
        size_t MappingCount = 0;

        friend class MemoryManager;

    };

    class MemoryManager : public Object {

    public:

        MemoryManager() = default;
        virtual ~MemoryManager() = default;

        bool Initialize();
        void* AdjustHeap(ptrdiff_t Increment);

        Page* AllocatePage(bool Zero = true);
        bool FreePage(Page& Page);

        bool MapPage(AddressSpace& AddressSpace, Page& Page, PagePermission Permissions, bool Pinned = false);
        bool UnmapPage(AddressSpace& AddressSpace, void* Address);
        bool MakeResident(AddressSpace& AddressSpace, void* Address);
        bool CloneAddressSpace(const AddressSpace& Source, AddressSpace& Destination);
        void ReleaseAddressSpace(AddressSpace& AddressSpace);

        bool ActivateAddressSpace(AddressSpace& AddressSpace);
        void DeactivateAddressSpace();
        AddressSpace* GetCurrentAddressSpace() const;

        PageFaultResult HandlePageFault(ExceptionFrame* Frame);
        const PageFaultInfo& GetLastPageFault() const;

        size_t GetFreePageCount() const;
        size_t GetResidentPageLimit() const;

    private:

        void DisableMPU();
        void EnableMPU();
        void DisableRegion(uint8_t Region);
        void ConfigureFlashRegion();
        bool LoadMapping(PageMapping& Mapping);
        uint32_t BuildRegionAttributes(PagePermission Permissions) const;

        Page Pages[PageCount] = {};
        PageMapping* ResidentMappings[8] = {};
        AddressSpace* CurrentAddressSpace = nullptr;
        PageFaultInfo LastPageFault = {};
        uint32_t UseCounter = 0;
        uintptr_t CurrentHeapEnd = 0;
        uint8_t HardwareRegions = 0;
        bool Initialized = false;

    };

};
