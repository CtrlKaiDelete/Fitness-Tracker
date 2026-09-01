#include <cstddef>
#include <cstdint>

extern "C" void* memset(void* Destination, int Value, std::size_t Size) {

    auto* Bytes = static_cast<uint8_t*>(Destination);
    for(std::size_t Index = 0; Index < Size; Index++) {
        Bytes[Index] = static_cast<uint8_t>(Value);
    }

    return Destination;

}
