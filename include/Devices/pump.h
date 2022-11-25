#pragma once

#include "Arduino.h"
#include "DeviceBase.h"
#include "./Models/Gpio/gpio.h"
#include "./Models/PID.h"

class Pump: public DeviceBase{
    private:
    // bool init_success = false;

    Gpio::GpioOutput dir_pin;
    Gpio::GpioOutput pwm_pin;
    int dir_pin_num;
    int pwm_pin_num;

    const int freq = 10000;
    const int pwm_channel = 0;
    const int resolution = 8;

    const float p_pompe = 70.0;
    const float i_pompe = 14.0;
    const float d_pompe = 0.0;

    public:
    virtual esp_err_t init();

    Pump(int _dir_pin, int _pwm_pin);

    PIDController<float> PIDPompe = PIDController<float>(p_pompe, i_pompe, d_pompe);


    esp_err_t send_command(int dutyCycle);
    esp_err_t stop();

    // bool is_init(){ return init_success; }


};
