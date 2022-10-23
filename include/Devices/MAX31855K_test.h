#pragma once

#include <Arduino.h>
#include "DeviceBase.h"
#include <SparkFunMAX31855k.h> // Using the max31855k driver
#include <SPI.h>  // Included here too due Arduino IDE; Used in above header



class max31855k_t: public DeviceBase{
  protected:
  SparkFunMAX31855k probe;

  public:

  virtual esp_err_t init();
  max31855k_t(uint8_t vcc, uint8_t sck, uint8_t miso, uint8_t cs);

  void test_read();
  float get_temp();
};
