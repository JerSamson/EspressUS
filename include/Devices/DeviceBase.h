#pragma once

#include "Arduino.h"

class DeviceBase
{
protected:
    bool init_success = false;

public:
    bool is_init(){ return init_success; }

    virtual esp_err_t init();
};

