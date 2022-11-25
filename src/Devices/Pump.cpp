#include "./Devices/pump.h"

Pump::Pump(int _dir_pin, int _pwm_pin): dir_pin(Gpio::GpioOutput((gpio_num_t)_dir_pin)), pwm_pin(Gpio::GpioOutput((gpio_num_t)_pwm_pin)), dir_pin_num(_dir_pin), pwm_pin_num(_pwm_pin){

}

esp_err_t Pump::send_command(int dutyCycle){
    if(!is_init()){
        Serial.println("ERROR\t- Pump::send_command - Tried to send command before successful initialization");
        return ESP_ERR_INVALID_STATE;
    }

    if(dutyCycle > 255){
        Serial.println("ERROR\t- Pump::send_command() - Given duty cycle exceeds limit (255)");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t status{ESP_OK};
    status |= dir_pin.set(false);  

    ledcWrite(pwm_channel, dutyCycle);  

    return status;
}

esp_err_t Pump::stop(){
    if(!is_init()){
        Serial.println("ERROR\t- Pump::stop() - Tried to send command before successful initialization");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t status{ESP_OK};

    ledcWrite(pwm_channel, 0);

    if(ESP_OK == status){
        Serial.println("INFO\t- Pump::stop() - Motor stopped");
    }else{
        Serial.println("ERROR\t- Pump::stop() - Couldn't set pwm pin to low");
    }

    return status;
}


esp_err_t Pump::init(){
    Serial.println("INFO\t- Pump::init() - Initializing Pump...");
    esp_err_t status{ESP_OK};

    status |= dir_pin.init();
    status |= pwm_pin.init();
    status |= dir_pin.set(false);

    // configure LED PWM functionalitites
    ledcSetup(pwm_channel, freq, resolution);
    
    // attach the channel to the GPIO to be controlled
    ledcAttachPin(pwm_pin_num, pwm_channel);

    init_success = ESP_OK == status;
    return status;
}