#pragma once

#include "Arduino.h"
#include "Sensor.h"
#include "./Models/Gpio/gpio.h"

class pressureSensor_t: public Sensor{
    private:
    Gpio::GpioADC adc;

    public:
    pressureSensor_t(gpio_num_t pin, adc1_channel_t channel , adc_bits_width_t width=ADC_WIDTH_12Bit, adc_attenuation_t attenuation=ADC_11db);

    virtual esp_err_t init();
    virtual float read(int readings=1);

    // virtual float get_pressure(); //TODO: Remove
};