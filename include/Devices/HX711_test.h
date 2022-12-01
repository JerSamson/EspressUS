#pragma once

#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "Sensor.h"
#include "./Models/Gpio/gpio.h"

class hx711_t: public Sensor{

protected:

uint8_t dout;
uint8_t sck;
Gpio::GpioOutput alim;

public:
HX711 scale;

bool is_active(){ return alim.state(); }
bool can_read(){ return is_active() && is_init() && scale.is_ready(); }

esp_err_t set(bool state){
    return alim.set(state);
}

bool toggle(){
    alim.toggle();
    return alim.state();
}

hx711_t(uint8_t _dout, uint8_t _sck, Gpio::GpioOutput _alim);
hx711_t(uint8_t _dout, uint8_t _sck, gpio_num_t _alim_pin);

virtual esp_err_t init();
virtual float read(int readings=1);

long zero();
void calibrate(float);
// float get_load(int=10); // TODO: remove

void test_read();

};