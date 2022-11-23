#pragma once

#include "Arduino.h"
#include "DeviceBase.h"
#include <ESP32CAN.h>
#include <chrono>
#include <CAN_config.h>

enum controlParam {
    position,
    vitesse,
    courant};

class VerinCan: public DeviceBase{
    private:
    std::chrono::_V2::system_clock::time_point last_wakeup_ms = std::chrono::_V2::system_clock::time_point::min();

    double get_ellapsed_ms_d(std::chrono::_V2::system_clock::time_point since){
        return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-since).count();
    }

    public:
    virtual esp_err_t init();
    CAN_frame_t wake_frame;
    CAN_frame_t send_frame;
    esp_err_t to_CAN(float targetPos, float currentLim, float dutyCycle, int mvtProfile, bool allowMvt);
    esp_err_t setup_frame_CAN();

    esp_err_t wake_up();
    // 1.5, 30.0, 0, 1
    esp_err_t send_CAN(float targetPos, float currentLim=3.4, float dutyCycle=30.0, int mvtProfile=0, bool allowMvt=1);
    int receive_CAN(CAN_frame_t rx_frame, controlParam option = position);

    VerinCan(gpio_num_t rx, gpio_num_t tx);
};
