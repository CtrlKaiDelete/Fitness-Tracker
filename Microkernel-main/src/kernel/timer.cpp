#include "Microkernel/include/kernel/timer.h"
#include "Microkernel/include/kernel/kernel.h"

#include <cstdint>

namespace Microkernel {

    Timer::Timer() {

        this->Ticks = 0;

    };

    uint64_t Timer::GetTicks() {

        return this->Ticks;

    };

    uint64_t Timer::GetMicroseconds() {

        uint32_t InterruptState = 0;
        __asm__ volatile("mrs %0, primask" : "=r"(InterruptState));
        __asm__ volatile("cpsid i" ::: "memory");
        *reinterpret_cast<volatile uint32_t*>(0x40008044) = 1;
        uint32_t SubMills = *reinterpret_cast<volatile uint32_t*>(0x40008544);
        uint64_t Millis = this->Ticks;
        if(*reinterpret_cast<volatile uint32_t*>(0x40008140) != 0) {

            Millis++;

        };

        if(InterruptState == 0) {

            __asm__ volatile("cpsie i" ::: "memory");

        };

        return Millis * 1000 + SubMills;

    };

    uint64_t Timer::GetMilliseconds() {

        return this->GetMicroseconds() / 1000;

    };

    uint64_t Timer::GetSeconds() {

        return this->GetMicroseconds() / 1000000;

    };

    void Timer::Start() {

        /*
         * Dear heavenly father I pray that my sanity may come back. Help me to read my
         * bible (Because bella wong is flaming me bc im not), and give me my ability
         * to get intimate and vulnerable w you back. Jesus name amen.
         */

        *reinterpret_cast<volatile uint32_t*>(0xE000E180) = 1u << 8;
        *reinterpret_cast<volatile uint32_t*>(0x40008004) = 1;
        *reinterpret_cast<volatile uint32_t*>(0x40008308) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(0x40008140) = 0;
        *reinterpret_cast<volatile uint32_t*>(0x40008504) = 0;
        *reinterpret_cast<volatile uint32_t*>(0x40008508) = 3;
        *reinterpret_cast<volatile uint32_t*>(0x40008510) = 4;
        *reinterpret_cast<volatile uint32_t*>(0x40008540) = 1000;
        *reinterpret_cast<volatile uint32_t*>(0x40008200) = 1;
        *reinterpret_cast<volatile uint32_t*>(0x4000800C) = 1;

        this->Ticks = 0;

        *reinterpret_cast<volatile uint32_t*>(0x40008304) = 1u << 16;
        *reinterpret_cast<volatile uint32_t*>(0xE000E280) = 1u << 8;
        *reinterpret_cast<volatile uint8_t*>(0xE000E400 + 8) = 0x80;
        *reinterpret_cast<volatile uint32_t*>(0xE000E100) = 1u << 8;
        *reinterpret_cast<volatile uint32_t*>(0x40008000) = 1;

    };

    void Timer::Stop() {

        *reinterpret_cast<volatile uint32_t*>(0xE000E180) = 1u << 8;
        *reinterpret_cast<volatile uint32_t*>(0x40008308) = 0xFFFFFFFF;
        *reinterpret_cast<volatile uint32_t*>(0x40008004) = 1;
        *reinterpret_cast<volatile uint32_t*>(0x40008140) = 0;
        *reinterpret_cast<volatile uint32_t*>(0xE000E280) = 1u << 8;

    };

    void Timer::HandleInterrupt() {

        volatile uint32_t* Event = reinterpret_cast<volatile uint32_t*>(0x40008140);

        if(*Event == 0) {

            return;

        };

        *Event = 0;
        (void)*Event;
        this->Ticks++;
        this->Kernel->Scheduler.Preempt();

    };

};

extern "C" void kernel_timer_dispatch() {

    Microkernel::Kernel::New()->InterruptTimer.HandleInterrupt();

};
