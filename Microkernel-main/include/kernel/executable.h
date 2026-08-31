#pragma once

#include <cstddef>

namespace Microkernel {

    using ExecutableEntry = int (*)(int, char**, char**);

    struct ExecutableArguments {

        static constexpr size_t MaximumArguments = 8;
        static constexpr size_t MaximumEnvironment = 8;
        static constexpr size_t MaximumStringLength = 64;

        char Arguments[MaximumArguments][MaximumStringLength] = {};
        char Environment[MaximumEnvironment][MaximumStringLength] = {};
        size_t ArgumentCount = 0;
        size_t EnvironmentCount = 0;

    };

};
