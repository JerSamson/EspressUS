#pragma once

#include "driver/gpio.h"

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

            virtual bool state(void) =0;
            virtual esp_err_t set(const bool state) =0;

            [[nodiscard]] esp_err_t init(void); // [[nodiscard]]: Throws error even if return value is not used

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

    // class GpioInput : public GpioBase
    // {
    //     private:
    //     /* Data */
    //     public:
    //         esp_err_t init(void);
    //         bool state(void);
    // }; // GpioInput
} // namespace GPIO