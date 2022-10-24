#pragma once

#include "Arduino.h"
#include "DeviceBase.h"
#include <ESP32CAN.h>
#include <CAN_config.h>


class VerinCan: public DeviceBase{

private:
uint8_t last_msg_ms = 0.0;

public:
virtual esp_err_t init();
CAN_frame_t wake_frame;
CAN_frame_t send_frame;
esp_err_t to_CAN(float targetPos, float currentLim, float dutyCycle, int mvtProfile, bool allowMvt);
esp_err_t setup_frame_CAN();

esp_err_t wake_up();
// 1.5, 30.0, 0, 1
esp_err_t send_CAN(float targetPos, float currentLim=3.0, float dutyCycle=30.0, int mvtProfile=0, bool allowMvt=1);

VerinCan(gpio_num_t rx, gpio_num_t tx);

};