#pragma once

#include <Arduino.h>
#include <functional>
#include <chrono>
#include <map>

#include "./Devices/Devices.h"
#include "./Models/BLE.h"
#include "Configuration.h"
#include "patch.h"
#include "State.h"

enum OperationMode{
    AUTO    = 0,
    MANUAL  = 1
};

class Controller final{
    private:

    static _Devices& Devices;
    static _Configuration& Configuration;

    bool first_loop = true;
    bool verin_moving_manual = false;

    OperationMode operation_mode = OperationMode::AUTO;

    esp_err_t status = ESP_OK;

    STATES current_state = WAIT_CLIENT;
    STATES last_state = WAIT_CLIENT;

    int loop_delay = 100;
    int exec_counter = 0;
    int exec_accum = 0;

    int pre_infusion_load_over_5g = 0;

    char err_log[100];
    bool err_resolved = true;

    int positionVerin = -1;
    float vitesseVerin;
    bool verinOn = true;

    std::chrono::_V2::system_clock::time_point last_exec = std::chrono::high_resolution_clock::now();

    //debug
    bool using_thermo = false;
    bool using_loadcell = false;
    bool using_lcd = false;
    bool using_pressure = true;
    //

    void print_once(std::string str);
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point last_execution = std::chrono::steady_clock::now();

    uint64_t ms_since_last_exec(){return get_ellapsed_ms(last_execution);}
    uint64_t ms_since_start(){return get_ellapsed_ms(begin);}

    void set_state(STATES state);

    bool update_value(
        Sensor sensor,
        float &current_value,
        std::map<uint64_t, float> &value_map,
        std::map<uint64_t, float> &d_map,
        std::map<uint64_t, float> &i_map
    );

    bool update_temp();
    bool update_load();
    bool update_pressure();

    // ====== Actions and transitions ====== 

    // Actions
    esp_err_t wait_client_action();
    esp_err_t idle_action();
    esp_err_t init_action();
    esp_err_t verin_up_action();
    esp_err_t pre_infusion_action();
    esp_err_t infusion_action();
    esp_err_t done_action();
    esp_err_t error_action();
    esp_err_t wait_porte_filtre_action();
    esp_err_t manual_action();

    // Transitions
    bool wait_client_transition();
    bool idle_to_init();
    bool init_to_verin_up();
    bool verin_up_to_pre_infusion();
    bool pre_infusion_to_infusion();
    bool infusion_to_enjoy();
    bool enjoy_to_main_menu();
    bool error_to_idle();
    bool waitpf_to_pre_infusion();
    bool waitpf_to_wait_client();
    bool connection_lost();


    // Values history
    float load = NAN;
    float temp = NAN;
    float pressure = NAN;

    std::map<uint64_t, float> temp_history;
    std::map<uint64_t, float> d_temp_history;
    std::map<uint64_t, float> i_temp_history;

    std::map<uint64_t, float> load_history;
    std::map<uint64_t, float> d_load_history;
    std::map<uint64_t, float> i_load_history;

    std::map<uint64_t, float> pressure_history;
    std::map<uint64_t, float> d_pressure_history;
    std::map<uint64_t, float> i_pressure_history;

    void update_values();
    float derivate(uint64_t time1, float value1, uint64_t time2, float value2, int time_factor = 1000);
    float integrate(uint64_t time1, float value1, uint64_t time2, float value2, int time_factor = 1000);

    public:

    //debug
    void debug_print_map(std::map<uint64_t, float> map);
    //


    void clear_history();
    void reset_time();

    int64_t get_ellapsed_ms(std::chrono::steady_clock::time_point since);
    
    esp_err_t execute();

    Controller();
  };
