#pragma once

#include "Microkernel/include/base/object.h"

namespace Microkernel {

    class DeviceManager;
    class Driver;

    class DriverWatchdogScope {

    public:

        explicit DriverWatchdogScope(Driver& Driver);
        DriverWatchdogScope(const DriverWatchdogScope&) = delete;
        ~DriverWatchdogScope();

    private:

        Driver* MonitoredDriver = nullptr;

    };

    class Driver : public Object {

    public:

        virtual void* Read() {

            return {};

        };

        virtual void Write(void*) {

        };

    protected:

        virtual void OnStart() {

        };

        virtual void OnStop() {

        };

        virtual void OnPanic() {

        };

        friend class DeviceManager;
        friend class DriverWatchdogScope;

    private:

        void StartDriver();
        void StopDriver();
        void ForceStopDriver();

        bool Running = false;

    };

};
