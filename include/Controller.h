#pragma once

#include <Arduino.h>
#include "./Devices/MAX31855K_test.h"
#include "./Devices/HX711_test.h"
#include "./Devices/sh110x_test.h"
#include <chrono>
#include <map>

enum STATES{
    IDLE,
    PRE_INFUSION,
    INFUSION,
    HEAT_1,
    HEAT_2,
    ERROR
};

class Controller{
    private:

    STATES current_state = STATES::IDLE;

    max31855k_t* thermocouple;
    hx711_t* loadCell;
    sh110x_t* lcd;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point last_execution = std::chrono::steady_clock::now();

    uint64_t ms_since_last_exec(){return get_ellapsed_ms(last_execution);}
    uint64_t ms_since_start(){return get_ellapsed_ms(begin);}

    float load = 0;
    float temp = 0;

    std::map<uint64_t, float> temp_history;
    std::map<uint64_t, float> load_history;
    
    public:

    void reset_time();
    int64_t get_ellapsed_ms(std::chrono::steady_clock::time_point since);
    void update_values();
    STATES execute();

    Controller(max31855k_t*, hx711_t*, sh110x_t*);
};