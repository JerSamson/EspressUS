#include <Arduino.h>
#include "./Devices/HX711_test.h"

hx711_t::hx711_t(uint8_t _dout, uint8_t _sck, Gpio::GpioOutput _alim) : dout(_dout), sck(_sck), alim(_alim){
}

hx711_t::hx711_t(uint8_t _dout, uint8_t _sck, gpio_num_t _alim_pin) : dout(_dout), sck(_sck), alim(Gpio::GpioOutput(_alim_pin)){
}

esp_err_t hx711_t::init(){
    Serial.println("INFO\t- hx711_t::init() - Initializing load cell...");

    esp_err_t status {ESP_OK};

    //Init Alimentation Gpio
    status |= alim.init();
    status |= alim.set(true);

    if(ESP_OK != status){
      Serial.println("ERROR\t- hx711_t::init() - Could not initialize loadCell's alimentation gpio.");
      init_success = false;
      return status;
    }

    // Init scale
    // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    scale.begin(dout, sck);

    init_success = true;
    return status;
}

float hx711_t::read(int readings){
    if(!is_active() || !is_init() || !scale.is_ready()){
    Serial.println("WARNING - hx711_t::get_load() - Tried to use loadCell but it is either inactive or not initialized.");
    return 0.0;
  }

  return scale.get_units(readings);
}

long hx711_t::zero(){
  if(!is_active() || !is_init() || !scale.is_ready()){
    Serial.println("WARNING - hx711_t::zero() - Tried to use loadCell but it is either inactive or not initialized.");
    return 0.0;
  }

  scale.tare();

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.printf("LOADCELL: Zero factor: %ld\n", zero_factor);
  return zero_factor;
}

void hx711_t::calibrate(float calibration_factor){
  if(!is_active() || !is_init() || !scale.is_ready()){
    Serial.println("WARNING - hx711_t::calibrate() - Tried to use loadCell but it is either inactive or not initialized.");
    return;
  }
  scale.set_scale(calibration_factor);
}

// float hx711_t::get_load(int readings){
//   if(!is_active() || !is_init() || !scale.is_ready()){
//     Serial.println("WARNING - hx711_t::get_load() - Tried to use loadCell but it is either inactive or not initialized.");
//     return 0.0;
//   }

//   return scale.get_units(readings);
// }

void hx711_t::test_read(){
  if(!is_active() || !is_init() || !scale.is_ready()){
    Serial.println("WARNING - hx711_t::test_read() - Tried to use loadCell but it is either inactive or not initialized.");
    return;
  }
  
  // 17.7g = 8000
  // 23.4g = 10500
  // 4.7g  = 2050

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
        Serial.println(reading/352.0);
        delay(500);
    }
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}