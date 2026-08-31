#pragma once

#include "Microkernel/include/interrupts/interrupt.h"
#include "Microkernel/include/base/object.h"

#include <vector>
#include <algorithm>

namespace Microkernel {

    class InterruptManager : public Object {

    public:

        template<typename T>
        T& RegisterInterrupt() {

            auto Iterator = std::find_if(this->Interrupts.begin(), this->Interrupts.end(), [](Interrupt* Interrupt){

                return dynamic_cast<T*>(Interrupt) != nullptr;

            });

            if(Iterator == this->Interrupts.end()) {

                T* D = new T();
                if(this->Interrupts.size() <= D->InterruptNumber) {

                    this->Interrupts.resize(D->InterruptNumber + 1, nullptr);

                };

                InterruptMask Mask;
                this->Interrupts[D->InterruptNumber] = D;
                __asm__ volatile("dsb" ::: "memory");
                *reinterpret_cast<volatile uint32_t*>(0xE000E100 + ((D->InterruptNumber / 32) * 4)) = 1u << (D->InterruptNumber % 32);
                return *D;

            } else {

                return *dynamic_cast<T*>(*Iterator);

            };

        };

        template<typename T>
        void UnregisterInterrupt() {

            auto Iterator = std::find_if(this->Interrupts.begin(), this->Interrupts.end(), [](Interrupt* Interrupt){

                return dynamic_cast<T*>(Interrupt);

            });

            if(Iterator == this->Interrupts.end()) {

                return;

            } else {

                std::erase(this->Interrupts, *Iterator);

            };

        };

        void HandleInterrupt(uint32_t Number);
        void DisableAll();

        InterruptManager() = default;
        virtual ~InterruptManager() = default;

    private:

        class InterruptMask {

        public:

            InterruptMask() {

                __asm__ volatile("mrs %0, primask" : "=r"(this->State));
                __asm__ volatile("cpsid i" ::: "memory");

            };

            ~InterruptMask() {

                if(this->State == 0) {

                    __asm__ volatile("cpsie i" ::: "memory");

                };

            };

        private:

            uint32_t State = 0;

        };

        std::vector<Interrupt*> Interrupts = {};

    };

};
