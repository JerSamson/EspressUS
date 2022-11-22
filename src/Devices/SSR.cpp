#include "./Devices/SSR.h"


SSR::SSR(gpio_num_t output, bool _initial_state): output_pin(Gpio::GpioOutput(output)), initial_state(_initial_state){
}

esp_err_t SSR::init(){
    Serial.println("INFO\t- SSR::init() - Initializing SSR...");
    esp_err_t status{ESP_OK};

    status |= output_pin.init();
    // status |= output_pin.set(initial_state);

    init_success = ESP_OK == status;

    return status;
}

esp_err_t SSR::set(bool state){
    if(!init_success){
         Serial.println("WARNING - SSR::set() - Tried to use SSR but it is not initialized.");
         return ESP_ERR_INVALID_STATE;
    }

    if(output_pin.state() == state){
        Serial.println("INFO\t- SSR::set() - Tried to set SSR to same state, skipping.");
        return ESP_OK;
    }

    return output_pin.set(state);
}