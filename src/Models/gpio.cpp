#include "./Models/Gpio/gpio.h"
#include <driver/adc.h>
#include <Arduino.h>

namespace Gpio
{
    // GpioBase
    esp_err_t GpioBase::init(void)
    {
        esp_err_t status{ESP_OK};

        status |= gpio_config(&_cfg);

        return status;
    }

    // GpioOutput
    esp_err_t GpioOutput::init(void)
    {
        Serial.printf("GpioOutput initializing pin %d\n", _pin);
        esp_err_t status{GpioBase::init()};

        if(ESP_OK == status){
            Serial.printf("GpioOutput initialized (pin: %d)\n", _pin);
            status |= set(_inverted_logic);
        }
        else{
            Serial.printf("GpioOutput NOT initialized (status: %d) (pin: %d)\n", _pin, status);
        }
        return status;
    }

    esp_err_t GpioOutput::set(const bool state)
    {
        Serial.printf("GpioOutput %d set to %s\n", _pin, (state == true) ? "HIGH" : "LOW");
        _state = state;
        return gpio_set_level(_pin,
            _inverted_logic ? !state : state);
    }

    // GpioADC
    int GpioADC::get_raw(void)
    {
        _value = analogRead(_pin);
        return _value;
    }

    float GpioADC::raw_to_voltage(int raw_data)
    {
        if(_width == ADC_WIDTH_12Bit)
            return raw_data * _vref/4095.0;
        else if (_width == ADC_WIDTH_11Bit)
            return raw_data * _vref/2047.0;
        else if (_width == ADC_WIDTH_10Bit)
            return raw_data * _vref/1023.0;
        else if (_width == ADC_WIDTH_9Bit)
            return raw_data * _vref/511.0;

        return 0.0;
    }

    esp_err_t GpioADC::init(void)
    {
        esp_err_t status{GpioBase::init()};
        
        // analogSetClockDiv(uint8_t clockDiv);

        // analogSetWidth(_width);
        // analogReadResolution(_width);
        // analogSetPinAttenuation(_pin, _attenuation);

        if(ESP_OK == status){
            Serial.printf("GpioADC initialized:\n\tpin: %d\n\twidth: %d\n\tatten: %d\n", _pin, _width, _attenuation);
        }else{
            Serial.println("ERROR - GpioADC::init() - Could not initialize ADC");
        }

        return status;
    }

    float GpioADC::voltage_to_pressure(float voltage)
    {
        const float sensor_vmin = 0.5;      // V
        const float sensor_vmax = 4.5;      // V
        const float sensor_pmax = 300;      // PSI
        const float a = sensor_pmax/(sensor_vmax-sensor_vmin);
        const float b = a*sensor_vmin;

        if(voltage <= sensor_vmin)
        {
            return 0;
        }
        return a*voltage - b;
    }

}
