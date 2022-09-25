#include "./MAX31855K_test.h"


max31855k_t::max31855k_t(uint8_t vcc, uint8_t sck, uint8_t miso, uint8_t cs) : probe(cs, vcc, sck, miso){
}

void max31855k_t::test_read()
{
    float temperature = probe.readCJT();

    // Read methods return NAN if they don't have a valid value to return.
    // The following conditionals only print the value if it's not NAN.
    if (!isnan(temperature)) {
      Serial.print("CJT is (˚C): ");
      Serial.println(temperature);
    }
    
    // Read the temperature in Celsius
    temperature = probe.readTempC();
    if (!isnan(temperature)) {
      Serial.print("Temp[C]=");
      Serial.print(temperature);
    }

    // Read the temperature in Fahrenheit
    temperature = probe.readTempF();
    if (!isnan(temperature)) {
      Serial.print("\tTemp[F]=");
      Serial.print(temperature);
    }

    // Read the temperature in Kelvin
    temperature = probe.readTempK();
    if (!isnan(temperature)) {
      Serial.print("\tTemp[K]=");
      Serial.print(temperature);
    }

    // Read the temperature in Rankine
    temperature = probe.readTempR();
    if (!isnan(temperature)) {
      Serial.print("\tTemp[R]=");
      Serial.println(temperature);
    }

    delay(750);
  }