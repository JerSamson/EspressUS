#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "./Models/Gpio/gpio.h"
#include "./Models/BLE/BLE_PressureService.h"
#include "./Models/BLE/BLE_base.h"

#define pdSECOND pdMS_TO_TICKS(1000)

class Main final
{
private:
    /* data */
public:
    esp_err_t setup(void);
    void run(void);

    // Test methods

        BLE::BLE_Pressure ble_pressure;

        // GpioOuput test
        // void blink(void); 
        // Gpio::GpioOutput led{GPIO_NUM_5};

        // GpioADC test 
        Gpio::GpioADC pressureSensor{GPIO_NUM_34, ADC1_CHANNEL_6, ADC_WIDTH_12Bit, ADC_11db};
        Gpio::GpioOutput analogReadTestPin{GPIO_NUM_26};

        float read_pressure(void); 

    Main(/* args */);
    ~Main();
};

Main::Main(/* args */)
{
}

Main::~Main()
{
}
