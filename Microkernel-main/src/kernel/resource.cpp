#include "Microkernel/include/kernel/resource.h"

namespace Microkernel {

    Resource::Resource(ResourceType Type) : Type(Type) {

    };

    void Resource::Retain() {

        this->References++;

    };

    void Resource::Release() {

        if(this->References > 0) {

            this->References--;

        };

    };

    size_t Resource::GetReferenceCount() const {

        return this->References;

    };

    ResourceType Resource::GetType() const {

        return this->Type;

    };

    uint32_t HandleTable::Add(Resource& Object, Capability Rights) {

        for(auto& Handle : this->Handles) {

            if(Handle.Object != nullptr) {

                continue;

            };

            Handle.Value = this->NextValue++;
            if(Handle.Value == 0) {

                Handle.Value = this->NextValue++;

            };

            Handle.Object = &Object;
            Handle.Rights = Rights;
            Object.Retain();
            return Handle.Value;

        };

        return 0;

    };

    Resource* HandleTable::Get(uint32_t Value, Capability Required, ResourceType Type) const {

        for(const auto& Handle : this->Handles) {

            if(Handle.Value != Value || Handle.Object == nullptr || !HasCapability(Handle.Rights, Required)) {

                continue;

            };

            if(Type != ResourceType::None && Handle.Object->GetType() != Type) {

                return nullptr;

            };

            return Handle.Object;

        };

        return nullptr;

    };

    bool HandleTable::Close(uint32_t Value) {

        for(auto& Handle : this->Handles) {

            if(Handle.Value != Value || Handle.Object == nullptr) {

                continue;

            };

            Handle.Object->Release();
            Handle = {};
            return true;

        };

        return false;

    };

    bool HandleTable::DuplicateTo(uint32_t Value, HandleTable& Destination, Capability Rights, uint32_t& NewValue) const {

        Resource* Object = this->Get(Value, Capability::Transfer);
        if(Object == nullptr) {

            return false;

        };

        for(const auto& Handle : this->Handles) {

            if(Handle.Value != Value || Handle.Object == nullptr) {

                continue;

            };

            if(!HasCapability(Handle.Rights, Rights)) {

                return false;

            };

            NewValue = Destination.Add(*Object, Rights);
            return NewValue != 0;

        };

        return false;

    };

    bool HandleTable::CloneTo(HandleTable& Destination) const {

        for(const auto& Handle : this->Handles) {

            if(Handle.Object == nullptr) {

                continue;

            };

            if(Destination.Add(*Handle.Object, Handle.Rights) == 0) {

                Destination.CloseAll();
                return false;

            };

        };

        return true;

    };

    void HandleTable::CloseAll() {

        for(auto& Handle : this->Handles) {

            if(Handle.Object != nullptr) {

                Handle.Object->Release();
                Handle = {};

            };

        };

    };

};
