#pragma once

#include "Arduino.h"
#include "./Models/Gpio/gpio.h"

class pressureSensor{
    private:
    Gpio::GpioADC _adc;

    bool init = false;

    public:
    bool is_init(){ return init;}
    pressureSensor(gpio_num_t, adc_channel_t);


};