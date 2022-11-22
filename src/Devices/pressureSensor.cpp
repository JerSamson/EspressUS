#include "./Devices/pressureSensor.h"

pressureSensor_t::pressureSensor_t(gpio_num_t pin, adc1_channel_t channel, adc_bits_width_t width, adc_attenuation_t attenuation): 
    adc(Gpio::GpioADC(pin, channel, width, attenuation)) {
}

esp_err_t pressureSensor_t::init(){
    Serial.println("INFO\t- pressureSensor_t::init() - Initializing pressure Sensor...");
    esp_err_t status{ESP_OK};
    status |= adc.init();

    init_success = ESP_OK == status;
    return status;
}

float pressureSensor_t::read(int readings){
    if(!is_init()){
      Serial.println("WARNING - pressureSensor_t::read() - Tried to read pressure sensor but it is not initialized.");
      return 0.0;
    }
    
    int raw_data = adc.get_raw();
    float voltage = adc.raw_to_voltage(raw_data);
    float pressure = adc.voltage_to_pressure(voltage);
    float pressure_bar = pressure/14.5;

    return pressure_bar;
}

// float pressureSensor_t::get_pressure(){
//     int raw_data = adc.get_raw();
//     float voltage = adc.raw_to_voltage(raw_data);
//     float pressure = adc.voltage_to_pressure(voltage);
//     float pressure_bar = pressure/14.5;

//     return pressure_bar;
// }

