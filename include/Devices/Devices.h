#pragma once

#include "Arduino.h"
#include "./Devices/HX711_test.h"
#include "./Devices/MAX31855K_test.h"
#include "./Devices/sh110x_test.h"
#include "./Devices/pressureSensor.h"
#include "./Devices/pump.h"
#include "./Devices/SSR.h"
#include "./Devices/VerinCan.h"

// LCD
#define LCD_ALIM_PIN GPIO_NUM_13
#define LCD_SDA_PIN 21
#define LCD_CLK_PIN 22

// LoadCell
#define LOADCELL_ALIM_PIN GPIO_NUM_25
#define LOADCELL_DOUT_PIN 27
#define LOADCELL_SCK_PIN 14

// Thermocouple
#define THERMO_VCC 4
#define THERMO_CS_PIN 5
#define THERMO_SCK_PIN 18
#define THERMO_MISO_PIN 19

// Verin
#define VERIN_CAN_RX GPIO_NUM_4
#define VERIN_CAN_TX GPIO_NUM_5

// Pressure Sensor
// ADC/Channel pairs : https://www.electronicshub.org/esp32-adc-tutorial/#:~:text=So%2C%20by%20default%2C%20the%20output,of%200V%20to%203.3V.
#define PRESSURESENSOR_ADC GPIO_NUM_32
#define PRESSURESENSOR_ADC_CH ADC1_CHANNEL_4
#define PRESSURESENSOR_ADC_WIDTH ADC_WIDTH_12Bit
#define PRESSURESENSOR_ADC_ATTENUATION ADC_11db

// Pump controller
#define PUMP_DIR_PIN 26
#define PUMP_PWM_PIN 15

// SSR TEST
#define SSR_TEST_PIN GPIO_NUM_23 


class _Devices: public DeviceBase{
    public:

    static _Devices& getInstance()
    {
        static _Devices instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
        return instance;
    }

    // _Devices(_Devices const&)        = delete;
    // void operator=(_Devices const&)  = delete;

    virtual esp_err_t init();

    static hx711_t loadCell;
    static max31855k_t thermocouple;
    static sh110x_t lcd;
    static pressureSensor_t pressureSensor;
    static Pump pump;
    static SSR testSSR;
    static VerinCan verin;

    private:
    static DeviceBase* init_list[];
    _Devices() {}
    // _Devices(_Devices const&);       // Don't Implement
    // void operator=(_Devices const&); // Don't implement
};

// static _Devices Devices;