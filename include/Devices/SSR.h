#pragma once

#include "./Models/Gpio/gpio.h"
#include "DeviceBase.h"

class SSR: public DeviceBase{
    private:
    
    Gpio::GpioOutput output_pin;
    bool initial_state;

    public:

    virtual esp_err_t init();

    bool is_init(){ return init_success; }
    bool get_state(){ return output_pin.state(); }

    esp_err_t set(bool state);

    SSR(gpio_num_t output, bool _initial_state = false);
};
