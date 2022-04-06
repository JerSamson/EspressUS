#include <Arduino.h>
#include "./Models/Gpio/gpio.h"
#include "./main.h"

static Main my_main;

void setup() {
  Serial.begin(115200);
  my_main.setup();
}

void loop() {
  my_main.run();
  vTaskDelay(pdSECOND);
}

void Main::run(void)
{
  float pressure = read_pressure();
  ble_pressure.update_pressure_value(pressure);
}

float Main::read_pressure(void)
{
    int raw_data = pressureSensor.get_raw();
    float voltage = pressureSensor.raw_to_voltage(raw_data);
    float pressure = pressureSensor.voltage_to_pressure(voltage);

    Serial.printf("Raw: %d\t Voltage: %f\tpressure: %f\n", raw_data, voltage, pressure);

    return pressure;
}

esp_err_t Main::setup()
{
    esp_err_t status {ESP_OK};

    Serial.println("Main Setup\n");

    status |= pressureSensor.init();
    status |= analogReadTestPin.init();

    analogReadTestPin.set(true);

    ble_pressure.add_characteristic("Test", BLECharacteristic::PROPERTY_READ);
    ble_pressure.update_characteristic("Test", "this is a test value");

    return status;
}