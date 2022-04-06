#pragma once

#include "driver/gpio.h"
#include <driver/adc.h>
#include <Arduino.h>

namespace Gpio
{

    class GpioBase
    {
        protected:
            const gpio_num_t _pin;
            const gpio_config_t _cfg;
            const bool _inverted_logic = false;

        public:

            constexpr GpioBase(const gpio_num_t pin,
                        const gpio_config_t& config, // &: copy cfg struct
                        const bool invert_logic = false) :
                _pin{pin},
                _cfg{config},
                _inverted_logic{invert_logic}
            {

            }

            esp_err_t init(void); // [[nodiscard]]: Throws error even if return value is not used

    }; // GpioBase

    class GpioOutput : public GpioBase
    {
        private:
            bool _state = false; // map the user's wish (regardless of inverted_logic)

        public:
            constexpr GpioOutput(const gpio_num_t pin, const bool invert = false) :
                GpioBase{pin, 
                gpio_config_t{
                    .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                    .mode           = GPIO_MODE_OUTPUT,
                    .pull_up_en     = GPIO_PULLUP_DISABLE,
                    .pull_down_en   = GPIO_PULLDOWN_ENABLE,
                    .intr_type      = GPIO_INTR_DISABLE
                },
                invert}
            {
                
            }

            esp_err_t init(void);

            esp_err_t set(const bool state);
            esp_err_t toggle(void);
            bool state(void) { return _state; }

    }; // GpioOutput

    class GpioInput : public GpioBase
    {
        protected:
            int _value = 0; 

        public:

        constexpr GpioInput(const gpio_num_t pin) :
                GpioBase{pin, 
                gpio_config_t{
                    .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                    .mode           = GPIO_MODE_INPUT,
                    .pull_up_en     = GPIO_PULLUP_DISABLE,
                    .pull_down_en   = GPIO_PULLDOWN_ENABLE,
                    .intr_type      = GPIO_INTR_DISABLE
                }}
            { }

        esp_err_t init(void);
        
    }; // GpioInput

    class GpioADC : public GpioBase
    {

        // TODO : Calibration ( https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html#adc-api-reference-adc-calibration )

        private:
            adc_attenuation_t _attenuation;
            adc1_channel_t _channel;
            adc_bits_width_t _width;
            float _vref;
            int _value = 0;

        public:
            
        constexpr GpioADC(const gpio_num_t pin,
                         const adc1_channel_t channel,
                         const adc_bits_width_t width = ADC_WIDTH_12Bit,
                         const adc_attenuation_t attenuation = ADC_11db,
                         const float vref = 3.3) :
                GpioBase{pin,               
                gpio_config_t{
                    .pin_bit_mask   = static_cast<uint64_t>(1) << pin,
                    .mode           = GPIO_MODE_INPUT,
                    .pull_up_en     = GPIO_PULLUP_DISABLE,
                    .pull_down_en   = GPIO_PULLDOWN_DISABLE,
                    .intr_type      = GPIO_INTR_DISABLE
                }},
                _attenuation{attenuation},
                _channel{channel},
                _width{width},
                _vref{vref}
                {}

        int get_raw(void);
        float raw_to_voltage(int);

        float voltage_to_pressure(float voltage);

        esp_err_t init(void);

    }; // GpioADC

} // namespace GPIO
