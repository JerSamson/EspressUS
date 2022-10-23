#pragma once

#include "Arduino.h"
#include "DeviceBase.h"
#include "./Models/Gpio/gpio.h"

class pressureSensor_t: public DeviceBase{
    private:
    Gpio::GpioADC adc;

    // bool init_success = false;

    public:
    pressureSensor_t(gpio_num_t pin, adc1_channel_t channel , adc_bits_width_t width=ADC_WIDTH_12Bit, adc_attenuation_t attenuation=ADC_11db);

    // bool is_init(){ return init_success;}
    virtual esp_err_t init();
    float get_pressure();
};