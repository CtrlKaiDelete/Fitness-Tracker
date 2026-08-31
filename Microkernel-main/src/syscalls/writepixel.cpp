#include "Microkernel/include/syscalls/writepixel.h"
#include "Microkernel/include/drivers/display.h"
#include "Microkernel/include/kernel/kernel.h"

#include <bit>

namespace Microkernel {

    uintptr_t WritePixel::Execute(uintptr_t Arg0, uintptr_t Arg1, uintptr_t Arg2) {

        int X = static_cast<int>(Arg0);
        int Y = static_cast<int>(Arg1);
        float Brightness = std::bit_cast<float>(static_cast<uint32_t>(Arg2));

        auto& Display = Kernel->DeviceManager.LoadDriver<DisplayDriver>();
        Display.WritePixel(X, Y, Brightness);

        return 0;

    };

};
