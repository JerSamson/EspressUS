#pragma once

#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "./Models/Gpio/gpio.h"

class hx711_t{

protected:

uint8_t dout;
uint8_t sck;
Gpio::GpioOutput alim;
HX711 scale;

bool init = false;

public:

bool is_active(){return alim.state();}
bool is_init(){ return init;}

bool toggle(){
    alim.toggle();
    return alim.state();
}

hx711_t(uint8_t _dout, uint8_t _sck, Gpio::GpioOutput _alim);
hx711_t(uint8_t _dout, uint8_t _sck, gpio_num_t _alim_pin);

esp_err_t setup(bool initial_state=true);

void zero();
void calibrate(float);
float get_load(int=10);

void test_read();

};