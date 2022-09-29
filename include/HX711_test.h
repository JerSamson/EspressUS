#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

class hx711_t{

protected:

uint8_t dout;
uint8_t sck;
HX711 scale;

public:

hx711_t(uint8_t _dout, uint8_t _sck);

void setup();

void zero();
void calibrate(float);
float get_load(int=10);

void test_read();



};