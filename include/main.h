#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "HX711.h"

#include "./Models/Gpio/gpio.h"
#include "./Models/BLE/BLE_PressureService.h"
#include "./Models/BLE/BLE_base.h"
#include "./Models/Controller.h"

#include "./Devices/Devices.h"

#define pdSECOND pdMS_TO_TICKS(1000)

class Main final
{
private:
    bool setup_success = false;
public:
    esp_err_t setup(void);
    void demo_edika();
    void run(void);

    _Devices& Devices = _Devices::getInstance();

    // Controller
    Controller controller = Controller();

    // Test methods

        // BLE::BLE_Pressure ble_pressure;
        // uint8_t i = decltype(i)(1);
        // uint16_t w = 128;
        // uint16_t h = 64;
        // uint8_t i = 1;
        // uint16_t w = 128;
        // uint16_t h = 64;

        // GpioOuput test
        // void blink(void); 
        // Gpio::GpioOutput led{GPIO_NUM_5};

        // GpioADC test 
        // Gpio::GpioADC pressureSensor{GPIO_NUM_34, ADC1_CHANNEL_6, ADC_WIDTH_12Bit, ADC_11db};
        // Gpio::GpioOutput analogReadTestPin{GPIO_NUM_26};

        // float read_pressure(void); 

    Main(/* args */);
    ~Main();
};

Main::Main(/* args */)
{
}

Main::~Main()
{
}
