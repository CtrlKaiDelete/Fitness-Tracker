#include "Microkernel/include/kernel/memory.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    extern "C" uint8_t __page_pool_start;
    extern "C" uint8_t __page_pool_end;
    extern "C" uint8_t __heap_start;
    extern "C" uint8_t __heap_end;

    static constexpr uintptr_t MPUType = 0xE000ED90;
    static constexpr uintptr_t MPUControl = 0xE000ED94;
    static constexpr uintptr_t MPURegionNumber = 0xE000ED98;
    static constexpr uintptr_t MPURegionBase = 0xE000ED9C;
    static constexpr uintptr_t MPURegionAttributes = 0xE000EDA0;
    static constexpr uintptr_t ConfigurableFaultStatus = 0xE000ED28;
    static constexpr uintptr_t MemoryFaultAddress = 0xE000ED34;

    static constexpr uint8_t FlashRegion = 0;
    static constexpr uint8_t FirstPageRegion = 1;
    static constexpr uint8_t MaximumHardwareRegions = 8;

    void* Page::GetAddress() const {

        return reinterpret_cast<void*>(this->Address);

    };

    bool Page::IsAllocated() const {

        return this->Allocated;

    };

    uintptr_t PageMapping::GetAddress() const {

        return this->PhysicalPage == nullptr ? 0 : reinterpret_cast<uintptr_t>(this->PhysicalPage->GetAddress());

    };

    PagePermission PageMapping::GetPermissions() const {

        return this->Permissions;

    };

    bool PageMapping::IsPresent() const {

        return this->Present;

    };

    bool PageMapping::IsResident() const {

        return this->MPURegion != InvalidMPURegion;

    };

    bool PageMapping::IsPinned() const {

        return this->Pinned;

    };

    PageMapping* AddressSpace::FindMapping(uintptr_t Address) {

        uintptr_t PageAddress = Address & ~(PageSize - 1);

        for(size_t Index = 0; Index < MaximumPageMappings; Index++) {

            PageMapping& Mapping = this->Mappings[Index];
            if(Mapping.Present && reinterpret_cast<uintptr_t>(Mapping.PhysicalPage->GetAddress()) == PageAddress) {

                return &Mapping;

            };

        };

        return nullptr;

    };

    const PageMapping* AddressSpace::FindMapping(uintptr_t Address) const {

        return const_cast<AddressSpace*>(this)->FindMapping(Address);

    };

    size_t AddressSpace::GetMappingCount() const {

        return this->MappingCount;

    };

    bool MemoryManager::Initialize() {

        if(this->Initialized) {

            return true;

        };

        uintptr_t PoolStart = reinterpret_cast<uintptr_t>(&__page_pool_start);
        uintptr_t PoolEnd = reinterpret_cast<uintptr_t>(&__page_pool_end);
        if(PoolEnd - PoolStart != PageCount * PageSize || (PoolStart & (PageSize - 1)) != 0) {

            return false;

        };

        uint32_t MPUDescription = *reinterpret_cast<volatile uint32_t*>(MPUType);
        this->HardwareRegions = (MPUDescription >> 8) & 0xFF;
        if(this->HardwareRegions > MaximumHardwareRegions) {

            this->HardwareRegions = MaximumHardwareRegions;

        };

        if(this->HardwareRegions <= FirstPageRegion) {

            return false;

        };

        for(size_t Index = 0; Index < PageCount; Index++) {

            this->Pages[Index].Address = PoolStart + (Index * PageSize);
            this->Pages[Index].Allocated = false;
            this->Pages[Index].References = 0;

        };

        this->DisableMPU();
        for(uint8_t Region = 0; Region < this->HardwareRegions; Region++) {

            this->DisableRegion(Region);

        };

        this->ConfigureFlashRegion();
        this->EnableMPU();
        this->Initialized = true;

        return true;

    };

    void* MemoryManager::AdjustHeap(ptrdiff_t Increment) {

        uint32_t InterruptState = 0;
        __asm__ volatile("mrs %0, primask" : "=r"(InterruptState));
        __asm__ volatile("cpsid i" ::: "memory");

        uintptr_t HeapStart = reinterpret_cast<uintptr_t>(&__heap_start);
        uintptr_t HeapEnd = reinterpret_cast<uintptr_t>(&__heap_end);
        if(this->CurrentHeapEnd == 0) {

            this->CurrentHeapEnd = HeapStart;

        };

        uintptr_t PreviousHeapEnd = this->CurrentHeapEnd;
        bool Invalid = false;

        if(Increment >= 0) {

            uintptr_t Increase = static_cast<uintptr_t>(Increment);
            Invalid = Increase > HeapEnd - this->CurrentHeapEnd;
            if(!Invalid) {

                this->CurrentHeapEnd += Increase;

            };

        } else {

            uintptr_t Decrease = static_cast<uintptr_t>(-(Increment + 1)) + 1;
            Invalid = Decrease > this->CurrentHeapEnd - HeapStart;
            if(!Invalid) {

                this->CurrentHeapEnd -= Decrease;

            };

        };

        if(InterruptState == 0) {

            __asm__ volatile("cpsie i" ::: "memory");

        };

        return Invalid ? nullptr : reinterpret_cast<void*>(PreviousHeapEnd);

    };

    Page* MemoryManager::AllocatePage(bool Zero) {

        if(!this->Initialized) {

            return nullptr;

        };

        for(auto& Page : this->Pages) {

            if(Page.Allocated) {

                continue;

            };

            Page.Allocated = true;
            Page.References = 0;

            if(Zero) {

                volatile uint32_t* Data = reinterpret_cast<volatile uint32_t*>(Page.Address);
                for(size_t Word = 0; Word < PageSize / sizeof(uint32_t); Word++) {

                    Data[Word] = 0;

                };

            };

            return &Page;

        };

        return nullptr;

    };

    bool MemoryManager::FreePage(Page& Page) {

        if(!Page.Allocated || Page.References != 0) {

            return false;

        };

        Page.Allocated = false;
        return true;

    };

    bool MemoryManager::MapPage(AddressSpace& AddressSpace, Page& Page, PagePermission Permissions, bool Pinned) {

        if(!this->Initialized || !Page.Allocated || !HasPermission(Permissions, PagePermission::Read)) {

            return false;

        };

        if(HasPermission(Permissions, PagePermission::Write) && HasPermission(Permissions, PagePermission::Execute)) {

            return false;

        };

        PageMapping* Existing = AddressSpace.FindMapping(Page.Address);
        if(Existing != nullptr) {

            Existing->Permissions = Permissions;
            Existing->Pinned = Pinned;
            if(Existing->IsResident() && this->CurrentAddressSpace == &AddressSpace) {

                this->LoadMapping(*Existing);

            };

            return true;

        };

        for(auto& Mapping : AddressSpace.Mappings) {

            if(Mapping.Present) {

                continue;

            };

            Mapping.PhysicalPage = &Page;
            Mapping.Permissions = Permissions;
            Mapping.LastUsed = 0;
            Mapping.MPURegion = InvalidMPURegion;
            Mapping.Present = true;
            Mapping.Pinned = Pinned;
            AddressSpace.MappingCount++;
            Page.References++;
            return true;

        };

        return false;

    };

    bool MemoryManager::UnmapPage(AddressSpace& AddressSpace, void* Address) {

        PageMapping* Mapping = AddressSpace.FindMapping(reinterpret_cast<uintptr_t>(Address));
        if(Mapping == nullptr) {

            return false;

        };

        if(Mapping->IsResident() && this->CurrentAddressSpace == &AddressSpace) {

            this->DisableRegion(Mapping->MPURegion);
            this->ResidentMappings[Mapping->MPURegion] = nullptr;

        };

        Mapping->PhysicalPage->References--;
        *Mapping = PageMapping();
        AddressSpace.MappingCount--;

        return true;

    };

    bool MemoryManager::MakeResident(AddressSpace& AddressSpace, void* Address) {

        if(this->CurrentAddressSpace != &AddressSpace) {

            return false;

        };

        PageMapping* Mapping = AddressSpace.FindMapping(reinterpret_cast<uintptr_t>(Address));
        if(Mapping == nullptr) {

            return false;

        };

        return this->LoadMapping(*Mapping);

    };

    bool MemoryManager::CloneAddressSpace(const AddressSpace& Source, AddressSpace& Destination) {

        if(Destination.MappingCount != 0) {

            return false;

        };

        for(const auto& Mapping : Source.Mappings) {

            if(!Mapping.Present) {

                continue;

            };

            if(!this->MapPage(Destination, *Mapping.PhysicalPage, Mapping.Permissions, Mapping.Pinned)) {

                this->ReleaseAddressSpace(Destination);
                return false;

            };

        };

        return true;

    };

    void MemoryManager::ReleaseAddressSpace(AddressSpace& AddressSpace) {

        for(auto& Mapping : AddressSpace.Mappings) {

            if(!Mapping.Present) {

                continue;

            };

            Page* PhysicalPage = Mapping.PhysicalPage;
            this->UnmapPage(AddressSpace, PhysicalPage->GetAddress());
            if(PhysicalPage->References == 0) {

                this->FreePage(*PhysicalPage);

            };

        };

    };

    bool MemoryManager::ActivateAddressSpace(AddressSpace& AddressSpace) {

        if(!this->Initialized) {

            return false;

        };

        this->DisableMPU();

        for(uint8_t Region = FirstPageRegion; Region < this->HardwareRegions; Region++) {

            this->DisableRegion(Region);
            this->ResidentMappings[Region] = nullptr;

        };

        if(this->CurrentAddressSpace != nullptr) {

            for(auto& Mapping : this->CurrentAddressSpace->Mappings) {

                Mapping.MPURegion = InvalidMPURegion;

            };

        };

        this->CurrentAddressSpace = &AddressSpace;
        this->ConfigureFlashRegion();
        this->EnableMPU();

        return true;

    };

    void MemoryManager::DeactivateAddressSpace() {

        this->DisableMPU();

        if(this->CurrentAddressSpace != nullptr) {

            for(auto& Mapping : this->CurrentAddressSpace->Mappings) {

                Mapping.MPURegion = InvalidMPURegion;

            };

        };

        for(uint8_t Region = FirstPageRegion; Region < this->HardwareRegions; Region++) {

            this->DisableRegion(Region);
            this->ResidentMappings[Region] = nullptr;

        };

        this->CurrentAddressSpace = nullptr;
        this->ConfigureFlashRegion();
        this->EnableMPU();

    };

    AddressSpace* MemoryManager::GetCurrentAddressSpace() const {

        return this->CurrentAddressSpace;

    };

    PageFaultResult MemoryManager::HandlePageFault(ExceptionFrame* Frame) {

        uint32_t FaultStatus = *reinterpret_cast<volatile uint32_t*>(ConfigurableFaultStatus) & 0xFF;
        uintptr_t Address = 0;

        if((FaultStatus & (1u << 0)) != 0 && Frame != nullptr) {

            Address = Frame->PC;

        } else if((FaultStatus & (1u << 7)) != 0) {

            Address = *reinterpret_cast<volatile uint32_t*>(MemoryFaultAddress);

        };

        this->LastPageFault.Address = Address;
        this->LastPageFault.FaultStatus = FaultStatus;

        if(this->CurrentAddressSpace == nullptr) {

            this->LastPageFault.Result = PageFaultResult::NoAddressSpace;
            return this->LastPageFault.Result;

        };

        PageMapping* Mapping = this->CurrentAddressSpace->FindMapping(Address);
        if(Mapping == nullptr) {

            this->LastPageFault.Result = PageFaultResult::InvalidAddress;
            return this->LastPageFault.Result;

        };

        if(Mapping->IsResident()) {

            this->LastPageFault.Result = PageFaultResult::PermissionDenied;
            return this->LastPageFault.Result;

        };

        if((FaultStatus & (1u << 0)) != 0 && !HasPermission(Mapping->Permissions, PagePermission::Execute)) {

            this->LastPageFault.Result = PageFaultResult::PermissionDenied;
            return this->LastPageFault.Result;

        };

        if(!this->LoadMapping(*Mapping)) {

            this->LastPageFault.Result = PageFaultResult::NoMPURegion;
            return this->LastPageFault.Result;

        };

        *reinterpret_cast<volatile uint32_t*>(ConfigurableFaultStatus) = FaultStatus;
        this->LastPageFault.Result = PageFaultResult::Handled;

        return this->LastPageFault.Result;

    };

    const PageFaultInfo& MemoryManager::GetLastPageFault() const {

        return this->LastPageFault;

    };

    size_t MemoryManager::GetFreePageCount() const {

        size_t FreePages = 0;
        for(const auto& Page : this->Pages) {

            if(!Page.Allocated) {

                FreePages++;

            };

        };

        return FreePages;

    };

    size_t MemoryManager::GetResidentPageLimit() const {

        return this->HardwareRegions > FirstPageRegion ? this->HardwareRegions - FirstPageRegion : 0;

    };

    void MemoryManager::DisableMPU() {

        __asm__ volatile("dmb" ::: "memory");
        *reinterpret_cast<volatile uint32_t*>(MPUControl) = 0;
        __asm__ volatile("dsb\nisb" ::: "memory");

    };

    void MemoryManager::EnableMPU() {

        *reinterpret_cast<volatile uint32_t*>(MPUControl) = (1u << 2) | 1u;
        __asm__ volatile("dsb\nisb" ::: "memory");

    };

    void MemoryManager::DisableRegion(uint8_t Region) {

        *reinterpret_cast<volatile uint32_t*>(MPURegionNumber) = Region;
        *reinterpret_cast<volatile uint32_t*>(MPURegionBase) = 0;
        *reinterpret_cast<volatile uint32_t*>(MPURegionAttributes) = 0;

    };

    void MemoryManager::ConfigureFlashRegion() {

        *reinterpret_cast<volatile uint32_t*>(MPURegionNumber) = FlashRegion;
        *reinterpret_cast<volatile uint32_t*>(MPURegionBase) = 0;
        *reinterpret_cast<volatile uint32_t*>(MPURegionAttributes) =
            (6u << 24) |
            (1u << 17) |
            (1u << 16) |
            (18u << 1) |
            1u;

    };

    bool MemoryManager::LoadMapping(PageMapping& Mapping) {

        uint8_t Region = InvalidMPURegion;

        if(Mapping.IsResident()) {

            Region = Mapping.MPURegion;

        } else {

            for(uint8_t Candidate = FirstPageRegion; Candidate < this->HardwareRegions; Candidate++) {

                if(this->ResidentMappings[Candidate] == nullptr) {

                    Region = Candidate;
                    break;

                };

            };

        };

        if(Region == InvalidMPURegion) {

            uint32_t OldestUse = UINT32_MAX;
            for(uint8_t Candidate = FirstPageRegion; Candidate < this->HardwareRegions; Candidate++) {

                PageMapping* Resident = this->ResidentMappings[Candidate];
                if(Resident != nullptr && !Resident->Pinned && Resident->LastUsed < OldestUse) {

                    OldestUse = Resident->LastUsed;
                    Region = Candidate;

                };

            };

        };

        if(Region == InvalidMPURegion) {

            return false;

        };

        PageMapping* Evicted = this->ResidentMappings[Region];
        if(Evicted != nullptr && Evicted != &Mapping) {

            Evicted->MPURegion = InvalidMPURegion;

        };

        this->DisableMPU();
        *reinterpret_cast<volatile uint32_t*>(MPURegionNumber) = Region;
        *reinterpret_cast<volatile uint32_t*>(MPURegionBase) = Mapping.PhysicalPage->Address;
        *reinterpret_cast<volatile uint32_t*>(MPURegionAttributes) = this->BuildRegionAttributes(Mapping.Permissions);
        this->EnableMPU();

        Mapping.MPURegion = Region;
        Mapping.LastUsed = ++this->UseCounter;
        this->ResidentMappings[Region] = &Mapping;

        return true;

    };

    uint32_t MemoryManager::BuildRegionAttributes(PagePermission Permissions) const {

        uint32_t AccessPermission = 0;
        bool User = HasPermission(Permissions, PagePermission::User);
        bool Writable = HasPermission(Permissions, PagePermission::Write);

        if(User) {

            AccessPermission = Writable ? 3u : 6u;

        } else {

            AccessPermission = Writable ? 1u : 5u;

        };

        uint32_t ExecuteNever = HasPermission(Permissions, PagePermission::Execute) ? 0u : 1u;

        return
            (ExecuteNever << 28) |
            (AccessPermission << 24) |
            (1u << 18) |
            (1u << 17) |
            (1u << 16) |
            (9u << 1) |
            1u;

    };

};
