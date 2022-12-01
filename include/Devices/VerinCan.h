#pragma once

#include "Arduino.h"
#include "DeviceBase.h"
#include "./Models/PID.h"
#include <ESP32CAN.h>
#include <chrono>
#include <CAN_config.h>
#include "./Models/PID.h"

enum controlParam {
    position,
    vitesse,
    courant};

class VerinCan: public DeviceBase{
    private:
    std::chrono::_V2::system_clock::time_point last_wakeup_ms = std::chrono::_V2::system_clock::now() - std::chrono::_V2::system_clock::duration(30000);
    CAN_frame_t rx_frame;

    const float p_verin = 8.0;
    const float i_verin = 0.006;
    const float d_verin = 0.06;

    public:

    static double get_ellapsed_ms_d(std::chrono::_V2::system_clock::time_point since){
        double ell = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-since).count();
        // Serial.printf("DEBUG\t- Get_ellapsed: %f\n", ell);
        return ell;
    }

    virtual esp_err_t init();
    CAN_frame_t wake_frame;
    CAN_frame_t send_frame;
    esp_err_t to_CAN(float targetPos, float currentLim, float dutyCycle, int mvtProfile, bool allowMvt);
    esp_err_t setup_frame_CAN();

    PIDController<float> PIDVerin = PIDController<float>(p_verin, i_verin, d_verin);

    esp_err_t wake_up();
    esp_err_t send_CAN(float targetPos, float dutyCycle=30.0, float currentLim=3.4, int mvtProfile=0, bool allowMvt=1);
    int receive_CAN(controlParam option = position);

    VerinCan(gpio_num_t rx, gpio_num_t tx);
};
