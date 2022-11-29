#pragma once

#include "DeviceBase.h"

class Sensor: public DeviceBase{
    public:
        virtual float read(int readings=1){
            throw std::exception();
        }
        virtual esp_err_t init(){
            throw std::exception();
        }
        virtual ~Sensor(){}
};