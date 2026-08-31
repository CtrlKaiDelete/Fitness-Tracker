// Created by Julien Rodot on 29/8/26. Licensed under MIT

#pragma once

#include <cstddef>
#include <cstdint>

namespace MEFI {

    constexpr uint32_t MakeSignature(char A, char B, char C, char D) {

        return static_cast<uint32_t>(A)
             | static_cast<uint32_t>(B) << 8
             | static_cast<uint32_t>(C) << 16
             | static_cast<uint32_t>(D) << 24;

    };

    constexpr uint32_t Revision = 0x00010000;
    constexpr uint32_t SystemTableSignature = MakeSignature('M', 'E', 'F', 'I');
    constexpr uint32_t BootServicesSignature = MakeSignature('M', 'E', 'B', 'S');
    constexpr uint32_t GraphicsOutputSignature = MakeSignature('M', 'E', 'G', 'O');

    enum class Status : uint32_t {

        Success = 0,
        InvalidParameter = 1,
        Unsupported = 2

    };

    struct TableHeader {

        uint32_t Signature;
        uint32_t Revision;
        uint32_t HeaderSize;
        uint32_t Reserved;

    };

    struct SystemTable;

    using StallFunction = void (*)(uint32_t Milliseconds);
    using ExitBootServicesFunction = void (*)(uintptr_t KernelVector, const SystemTable* Table);
    using SetPixelFunction = Status (*)(uint32_t X, uint32_t Y, uint32_t Enabled);
    using ClearFunction = void (*)();
    using WriteFrameFunction = Status (*)(const uint8_t* Rows);

    struct BootServices {

        TableHeader Header;
        StallFunction Stall;
        ExitBootServicesFunction ExitBootServices;

    };

    struct GraphicsOutput {

        TableHeader Header;
        uint32_t Width;
        uint32_t Height;
        uint32_t PixelsPerScanLine;
        volatile uint8_t* Framebuffer;
        SetPixelFunction SetPixel;
        ClearFunction Clear;
        WriteFrameFunction WriteFrame;

    };

    struct SystemTable {

        TableHeader Header;
        const BootServices* Boot;
        const GraphicsOutput* Graphics;
        uintptr_t KernelVector;
        uintptr_t FirmwareBase;
        std::size_t FirmwareSize;

        [[nodiscard]] bool IsValid() const {

            return Header.Signature == SystemTableSignature
                && Header.Revision >= Revision
                && Header.HeaderSize >= sizeof(SystemTable)
                && Boot != nullptr
                && Graphics != nullptr;

        };

    };

    static_assert(sizeof(void*) == sizeof(uint32_t));
    static_assert(sizeof(TableHeader) == 16);
    static_assert(sizeof(BootServices) == 24);
    static_assert(sizeof(GraphicsOutput) == 44);
    static_assert(sizeof(SystemTable) == 36);

};
