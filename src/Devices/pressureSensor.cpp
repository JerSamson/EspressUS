#include "./Devices/pressureSensor.h"

pressureSensor_t::pressureSensor_t(gpio_num_t pin, adc1_channel_t channel , adc_bits_width_t width, adc_attenuation_t attenuation): adc(Gpio::GpioADC(pin, channel, width, attenuation)) {
}

esp_err_t pressureSensor_t::init(){
    esp_err_t status{ESP_OK};
    status |= adc.init();

    init_success = ESP_OK == status;
    return status;
}

float pressureSensor_t::get_pressure(){
    int raw_data = adc.get_raw();
    float voltage = adc.raw_to_voltage(raw_data);
    float pressure = adc.voltage_to_pressure(voltage);

    Serial.printf("PRESSURE SENSOR - Raw: %d\t Voltage: %2fV\tpressure: %2fPSI\n", raw_data, voltage, pressure);

    return pressure;
}