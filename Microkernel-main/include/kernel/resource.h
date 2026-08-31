#pragma once

#include "Microkernel/include/base/object.h"

#include <cstddef>
#include <cstdint>

namespace Microkernel {

    enum class ResourceType : uint8_t {

        None,
        Endpoint,
        File

    };

    enum class Capability : uint8_t {

        None = 0,
        Read = 1u << 0,
        Write = 1u << 1,
        Transfer = 1u << 2,
        Manage = 1u << 3

    };

    constexpr Capability operator|(Capability Left, Capability Right) {

        return static_cast<Capability>(static_cast<uint8_t>(Left) | static_cast<uint8_t>(Right));

    };

    constexpr bool HasCapability(Capability Value, Capability Required) {

        return (static_cast<uint8_t>(Value) & static_cast<uint8_t>(Required)) == static_cast<uint8_t>(Required);

    };

    class Resource : public Object {

    public:

        explicit Resource(ResourceType Type);
        virtual ~Resource() = default;
        void Retain();
        void Release();
        size_t GetReferenceCount() const;
        ResourceType GetType() const;

    private:

        size_t References = 0;
        ResourceType Type = ResourceType::None;

    };

    struct Handle {

        uint32_t Value = 0;
        Resource* Object = nullptr;
        Capability Rights = Capability::None;

    };

    class HandleTable {

    public:

        static constexpr size_t MaximumHandles = 32;
        uint32_t Add(Resource& Object, Capability Rights);
        Resource* Get(uint32_t Value, Capability Required, ResourceType Type = ResourceType::None) const;
        bool Close(uint32_t Value);
        bool DuplicateTo(uint32_t Value, HandleTable& Destination, Capability Rights, uint32_t& NewValue) const;
        bool CloneTo(HandleTable& Destination) const;
        void CloseAll();

    private:

        Handle Handles[MaximumHandles] = {};
        uint32_t NextValue = 3;

    };

};
