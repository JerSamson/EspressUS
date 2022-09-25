#pragma once


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "./Models/Gpio/gpio.h"
#include "./Models/BLE/BLE_PressureService.h"
#include "./Models/BLE/BLE_base.h"
#include "./HX711_test.h"
#include "./MAX31855K_test.h"
#include "./sh110x_test.h"
// #include "./Adafruit_SSD1306/Adafruit_SSD1306.h"
// #include "./Adafruit-GFX-Library/Adafruit_GrayOLED.cpp"
#include <SPI.h>
#include <Adafruit_GFX.h>
// #include "./Adafruit-GFX-Library/Adafruit_SPITFT.h"
// #include "Adafruit_SH110X.h"
#include <Wire.h>
#include "HX711.h"

#define pdSECOND pdMS_TO_TICKS(1000)

// LCD
#define LCD_ALIM_PIN GPIO_NUM_13
#define LCD_SDA_PIN 8
#define LCD_CLK_PIN 6

// #define OLED_MOSI     12
// #define OLED_CLK      14
// #define OLED_DC       7
// #define OLED_CS       5
// #define OLED_RST      9

// LoadCell
#define LOADCELL_ALIM_PIN GPIO_NUM_25
#define LOADCELL_DOUT_PIN 27
#define LOADCELL_SCK_PIN 14

// Thermocouple
#define THERMO_VCC 4
#define THERMO_CS_PIN 5
#define THERMO_SCK_PIN 18
#define THERMO_MISO_PIN 19


class Main final
{
private:
    /* data */
public:
    esp_err_t setup(void);
    void run(void);

    // Load Cell
    Gpio::GpioOutput loadCellAlim = Gpio::GpioOutput(LOADCELL_ALIM_PIN);
    hx711_t loadCell = hx711_t(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

    // Thermocouple
    max31855k_t thermo = max31855k_t(THERMO_VCC, THERMO_SCK_PIN, THERMO_MISO_PIN, THERMO_CS_PIN);

    // LCD
    Gpio::GpioOutput lcdAlim = Gpio::GpioOutput(LCD_ALIM_PIN);
    sh110x_t lcd = sh110x_t();

    // Test methods

        // BLE::BLE_Pressure ble_pressure;
        uint8_t i = decltype(i)(1);
        uint16_t w = 128;
        uint16_t h = 64;
        // uint8_t i = 1;
        // uint16_t w = 128;
        // uint16_t h = 64;

        // GpioOuput test
        // void blink(void); 
        // Gpio::GpioOutput led{GPIO_NUM_5};

        // GpioADC test 
        // Gpio::GpioADC pressureSensor{GPIO_NUM_34, ADC1_CHANNEL_6, ADC_WIDTH_12Bit, ADC_11db};
        // Gpio::GpioOutput analogReadTestPin{GPIO_NUM_26};

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
