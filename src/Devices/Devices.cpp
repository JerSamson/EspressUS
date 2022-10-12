#include "./Devices/Devices.h"


hx711_t _Devices::loadCell = hx711_t(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, LOADCELL_ALIM_PIN);
max31855k_t _Devices::thermocouple = max31855k_t(THERMO_VCC, THERMO_SCK_PIN, THERMO_MISO_PIN, THERMO_CS_PIN);
sh110x_t _Devices::lcd = sh110x_t(LCD_SDA_PIN, LCD_CLK_PIN, LCD_ALIM_PIN);
pressureSensor_t _Devices::pressureSensor = pressureSensor_t(PRESSURESENSOR_ADC, PRESSURESENSOR_ADC_CH, PRESSURESENSOR_ADC_WIDTH, PRESSURESENSOR_ADC_ATTENUATION);

