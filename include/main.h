#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <SPI.h>
#include <string> 
#include <sstream> 
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "HX711.h"

#include "./Models/Gpio/gpio.h"
#include "./Models/BLE/BLE_base.h"
#include "./Models/Controller.h"
#include "patch.h"
#include "./Devices/Devices.h"

#define pdSECOND pdMS_TO_TICKS(1000)

class Main final
{
private:
    bool setup_success = false;
public:
    esp_err_t setup(void);
    void demo_edika();
    void demo_tech();
    void test_devices_freq();

    void test_ble();
    void test_ble_float();
    void test_ble_state();
    void test_ble_load();
    void test_ble_temp();
    void test_ble_dynamic_plot();
    void test_user_action();
    
    bool wait_for_user_action(std::string ua = "StartApp");

    void run(void);

    _Devices& Devices = _Devices::getInstance();

    // Controller
    Controller controller = Controller();

    BLE_Base ble = BLE_Base();

    Main(/* args */);
    ~Main();
};

Main::Main(/* args */)
{
}

Main::~Main()
{
}
