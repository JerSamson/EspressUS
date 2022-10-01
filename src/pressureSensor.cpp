#include "./Devices/pressureSensor.h"


pressureSensor::pressureSensor(gpio_num_t adc, adc_channel_t adc_channel): _adc(Gpio::GpioADC(GPIO_NUM_34, ADC1_CHANNEL_6, ADC_WIDTH_12Bit, ADC_11db)) {
}
