#pragma once

#include "DeviceBase.h"

class Sensor: public DeviceBase{
    public:
        virtual float read(int readings=1);
};