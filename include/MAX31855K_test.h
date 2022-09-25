#include <Arduino.h>
#include <SparkFunMAX31855k.h> // Using the max31855k driver
#include <SPI.h>  // Included here too due Arduino IDE; Used in above header

class max31855k_t{
  protected:
  SparkFunMAX31855k probe;

  public:

  max31855k_t(uint8_t vcc, uint8_t sck, uint8_t miso, uint8_t cs);

  void test_read();

};
