#include "Microkernel/include/drivers/driver.h"
#include "Microkernel/include/kernel/kernel.h"

namespace Microkernel {

    DriverWatchdogScope::DriverWatchdogScope(Driver& Driver) {

        this->MonitoredDriver = &Driver;
        if(Driver.Kernel != nullptr) {

            Driver.Kernel->DriverWatchdog.Arm(Driver);

        };

    };

    DriverWatchdogScope::~DriverWatchdogScope() {

        if(this->MonitoredDriver != nullptr && this->MonitoredDriver->Kernel != nullptr) {

            this->MonitoredDriver->Kernel->DriverWatchdog.Disarm(*this->MonitoredDriver);

        };

    };

    void Driver::StartDriver() {

        if(this->Running) {

            return;

        };

        DriverWatchdogScope Watchdog(*this);
        this->OnStart();
        this->Running = true;

    };

    void Driver::StopDriver() {

        if(!this->Running) {

            return;

        };

        DriverWatchdogScope Watchdog(*this);
        this->OnStop();
        this->Running = false;

    };

    void Driver::ForceStopDriver() {

        this->Running = false;
        this->OnPanic();

    };

};
