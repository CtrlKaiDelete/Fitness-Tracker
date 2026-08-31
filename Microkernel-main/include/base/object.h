#pragma once

namespace Microkernel {

    class Kernel;
    class Object {

    public:

        Object() = default;
        virtual ~Object() = default;

        friend class Kernel;
        inline static Microkernel::Kernel* Kernel = nullptr;

    private:

    };

};
