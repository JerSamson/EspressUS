#pragma once

#include <Arduino.h>
#include "State.h"
#include "./Devices/Devices.h"
#include <chrono>
#include <map>
#include "Configuration.h"



class Controller final{
    private:
    static _Devices& Devices;
    static _Configuration& Configuration;

    esp_err_t status = ESP_OK;
    State current_state = idle_state;

    //debug
    bool first_itt = true;
    bool using_thermo = false;
    bool using_loadcell = false;
    bool using_lcd = false;
    bool using_pressure = true;
    //
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point last_execution = std::chrono::steady_clock::now();

    uint64_t ms_since_last_exec(){return get_ellapsed_ms(last_execution);}
    uint64_t ms_since_start(){return get_ellapsed_ms(begin);}

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
    
    public:

    //debug
    void debug_print_map(std::map<uint64_t, float> map);
    //

    State get_next_state() const;

    float derivate(uint64_t time1, float value1, uint64_t time2, float value2, int time_factor = 1000);
    float integrate(uint64_t time1, float value1, uint64_t time2, float value2, int time_factor = 1000);

    void clear_history();
    void reset_time();
    int64_t get_ellapsed_ms(std::chrono::steady_clock::time_point since);

    void update_values();
    
    esp_err_t execute();

    Controller();


    // ====== States and transitions ====== 

    // Actions
    static esp_err_t idle_action();
    static esp_err_t init_action();
    static esp_err_t heating_action();
    static esp_err_t fill_boiler_action();
    static esp_err_t ready_action();
    static esp_err_t verin_up_action();
    static esp_err_t filling_head_action();
    static esp_err_t extract_action();
    static esp_err_t done_action();
    static esp_err_t choke_action();
    static esp_err_t dripping_action();
    static esp_err_t flush_action();
    static esp_err_t error_action();

    // Transitions
    static bool idle_to_init();
    static bool init_to_heating();
    static bool heating_to_ready();
    static bool heating_to_fill_boiler();
    static bool fill_boiler_to_heating();
    static bool ready_to_fill_boiler();
    static bool ready_to_verin_up();
    static bool verin_up_to_fill_head();
    static bool fill_head_to_extract();
    static bool extract_to_enjoy();
    static bool extract_to_choke();
    static bool enjoy_to_dripping();
    static bool choke_to_ready();
    static bool choke_to_flush();
    static bool dripping_to_ready();
    static bool dripping_to_flush();
    static bool flush_to_ready();
    static bool error_to_idle();

    // States
    const std::map<STATES, State> state_map = {
        {STATES::IDLE, idle_state},
        {STATES::INIT, init_state},
        {STATES::HEATING, heating_state},
        {STATES::FILL_BOILER, fill_boiler_state},
        {STATES::READY, ready_state},
        {STATES::VERIN_UP, verin_up_state},
        {STATES::FILLING_HEAD, filling_head_state},
        {STATES::EXTRACT, extract_state},
        {STATES::DONE, enjoy_state},
        {STATES::CHOKE, choke_state},
        {STATES::DRIPPING, dripping_state},
        {STATES::FLUSH, flush_state},
        {STATES::ERROR, error_state}
    };

    State error_state = {
        state: STATES::ERROR,
        transitions: {
            {
                next_state: STATES::INIT,
                condition: Controller::error_to_idle
            }
        },
        action: &Controller::error_action
    };

    State idle_state = {
        state: STATES::IDLE,
        transitions: {
            {
                next_state: STATES::INIT,
                condition: Controller::idle_to_init
            }
        },
        action: &Controller::idle_action
    };

    State init_state = {
        state: STATES::INIT,
        transitions: {
            {
                next_state: STATES::HEATING,
                condition: Controller::init_to_heating
            }
        },
        action: &Controller::init_action
    };

    State heating_state = {
        state: STATES::HEATING,
        transitions: {
            {
                next_state: STATES::READY,
                condition: Controller::heating_to_ready
            },
            {
                next_state: STATES::FILL_BOILER,
                condition: Controller::heating_to_fill_boiler
            }
            // heating_to_ready,
            // heating_to_fill_boiler
        },
        action: &Controller::heating_action
    };

    State fill_boiler_state = {
        state: STATES::FILL_BOILER,
        transitions: {
            {
                next_state: STATES::HEATING,
                condition: Controller::fill_boiler_to_heating
            }
        },
        action: &Controller::fill_boiler_action
    };

    State ready_state = {
        state: STATES::READY,
        transitions: {
            {
                next_state: STATES::FILL_BOILER,
                condition: Controller::ready_to_fill_boiler
            },
            {
                next_state: STATES::VERIN_UP,
                condition: Controller::ready_to_verin_up
            }
            // ready_to_fill_boiler,
            // ready_to_verin_up
        },
        action: &Controller::ready_action
    };

    State verin_up_state = {
        state: STATES::VERIN_UP,
        transitions: {
            {
                next_state: STATES::FILLING_HEAD,
                condition: Controller::verin_up_to_fill_head
            }
            // verin_up_to_fill_head
        },
        action: &Controller::verin_up_action
    };

    State filling_head_state = {
        state: STATES::FILLING_HEAD,
        transitions: {
            {
                next_state: STATES::EXTRACT,
                condition: Controller::fill_head_to_extract
            }
            // fill_head_to_extract
        },
        action: &Controller::filling_head_action
    };

    State extract_state = {
        state: STATES::EXTRACT,
        transitions: {
            {
                next_state: STATES::DONE,
                condition: Controller::extract_to_enjoy
            },
            {
                next_state: STATES::CHOKE,
                condition: Controller::extract_to_choke
            }
            // extract_to_enjoy,
            // extract_to_choke
        },
        action: &Controller::extract_action
    };

    State enjoy_state = {
        state: STATES::DONE,
        transitions: {
            {
                next_state: STATES::DRIPPING,
                condition: Controller::enjoy_to_dripping
            }
            // enjoy_to_dripping
        },
        action: &Controller::done_action
    };

    State choke_state = {
        state: STATES::CHOKE,
        transitions: {
            {
                next_state: STATES::READY,
                condition: Controller::choke_to_ready
            },
            {
                next_state: STATES::FLUSH,
                condition: Controller::choke_to_flush
            }
            // choke_to_ready,
            // choke_to_flush
        },
        action: &Controller::choke_action
    };

    State dripping_state = {
        state: STATES::DRIPPING,
        transitions: {
            {
                next_state: STATES::READY,
                condition: Controller::dripping_to_ready
            },
            {
                next_state: STATES::FLUSH,
                condition: Controller::dripping_to_flush
            }
            // dripping_to_ready,
            // dripping_to_flush
        },
        action: &Controller::dripping_action
    };
    
    State flush_state = {
        state: STATES::FLUSH,
        transitions: {
            {
                next_state: STATES::READY,
                condition: Controller::flush_to_ready
            }
            // flush_to_ready
        },
        action: &Controller::flush_action
    };
};
