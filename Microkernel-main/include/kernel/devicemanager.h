#pragma once

#include "Microkernel/include/drivers/driver.h"
#include "Microkernel/include/base/object.h"

#include <vector>
#include <algorithm>

namespace Microkernel {

    class DeviceManager : public Object {

    public:

        template<typename T>
        T& LoadDriver() {

            auto Iterator = std::find_if(this->Drivers.begin(), this->Drivers.end(), [](Driver* Driver){

                return dynamic_cast<T*>(Driver) != nullptr;

            });

            if(Iterator == this->Drivers.end()) {

                T* D = new T();
                this->Drivers.push_back(D);

                static_cast<Driver*>(D)->StartDriver();
                return *D;

            } else {

                return *dynamic_cast<T*>(*Iterator);

            };

        };

        template<typename T>
        void UnloadDriver() {

            auto Iterator = std::find_if(this->Drivers.begin(), this->Drivers.end(), [](Driver* Driver){

                return dynamic_cast<T*>(Driver);

            });

            if(Iterator == this->Drivers.end()) {

                return;

            } else {

                (*Iterator)->StopDriver();
                std::erase(this->Drivers, *Iterator);

            };

        };

        void ForceStopAll() {

            for(auto Iterator = this->Drivers.rbegin(); Iterator != this->Drivers.rend(); Iterator++) {

                if(*Iterator != nullptr) {

                    (*Iterator)->ForceStopDriver();

                };

            };

        };

        DeviceManager() = default;
        virtual ~DeviceManager() = default;

    private:

        std::vector<Driver*> Drivers = {};

    };

};
