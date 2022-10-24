#include "./Devices/Devices.h"

pressureSensor_t _Devices::pressureSensor = pressureSensor_t(PRESSURESENSOR_ADC, PRESSURESENSOR_ADC_CH, PRESSURESENSOR_ADC_WIDTH, PRESSURESENSOR_ADC_ATTENUATION);
hx711_t          _Devices::loadCell       = hx711_t(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, LOADCELL_ALIM_PIN);
max31855k_t      _Devices::thermocouple   = max31855k_t(THERMO_VCC, THERMO_SCK_PIN, THERMO_MISO_PIN, THERMO_CS_PIN);
sh110x_t         _Devices::lcd            = sh110x_t(LCD_SDA_PIN, LCD_CLK_PIN, LCD_ALIM_PIN);
Pump             _Devices::pump           = Pump(PUMP_DIR_PIN, PUMP_PWM_PIN);
SSR              _Devices::testSSR        = SSR(SSR_TEST_PIN);
VerinCan         _Devices::verin          = VerinCan(VERIN_CAN_RX, VERIN_CAN_TX);

DeviceBase* _Devices::init_list[10]  = {
        &loadCell,
        &thermocouple,
        &lcd,
        &pressureSensor,
        &pump,
        &testSSR,
        &verin
    };

esp_err_t _Devices::init(){
    esp_err_t status{ESP_OK};
    Serial.println("INFO - _Devices::init() - Initializing all devices in init_list");
    for (DeviceBase* device : init_list) {
        if(device != nullptr && !device->is_init()){
            status |= device->init();
        }
    }

    init_success = status == ESP_OK;

    Serial.printf("%s - _Devices::init() - Initialization finished. %ss\n", init_success ? "INFO" : "ERROR", init_success ? "SUCCEEDED" : "FAILED" );

    return status;
}