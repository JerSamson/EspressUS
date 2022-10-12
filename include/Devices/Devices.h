#pragma once

#include "./Devices/HX711_test.h"
#include "./Devices/MAX31855K_test.h"
#include "./Devices/sh110x_test.h"
#include "./Devices/pressureSensor.h"

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

// Pressure Sensor
// ADC/Channel pairs : https://www.electronicshub.org/esp32-adc-tutorial/#:~:text=So%2C%20by%20default%2C%20the%20output,of%200V%20to%203.3V.
#define PRESSURESENSOR_ADC GPIO_NUM_32
#define PRESSURESENSOR_ADC_CH ADC1_CHANNEL_4
#define PRESSURESENSOR_ADC_WIDTH ADC_WIDTH_12Bit
#define PRESSURESENSOR_ADC_ATTENUATION ADC_11db

class _Devices{
    public:

    static _Devices& getInstance()
    {
        static _Devices instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
        return instance;
    }

    // _Devices(_Devices const&)        = delete;
    // void operator=(_Devices const&)  = delete;

    static hx711_t loadCell;
    static max31855k_t thermocouple;
    static sh110x_t lcd;
    static pressureSensor_t pressureSensor;

    private:

    _Devices() {}
    // _Devices(_Devices const&);       // Don't Implement
    // void operator=(_Devices const&); // Don't implement
};

// static _Devices Devices;