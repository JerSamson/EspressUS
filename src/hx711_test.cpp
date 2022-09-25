#include <Arduino.h>
#include "./HX711_test.h"

hx711_t::hx711_t(uint8_t _dout, uint8_t _sck) : dout(_dout), sck(_sck){
}

void hx711_t::setup(){
    Serial.begin(115200);
    //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    scale.begin(dout, sck);
}

void hx711_t::test_read(){
  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    while(true)
    {
        long reading = scale.get_units(10);
        Serial.print("Result: ");
        Serial.println(reading);
        delay(500);
    }

  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}