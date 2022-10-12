#include <map>
#include <functional>
#include "Arduino.h"

enum STATES{
    IDLE,
    INIT,
    HEATING,
    FILL_BOILER,
    READY,
    VERIN_UP,
    FILLING_HEAD,
    EXTRACT,
    DONE,
    CHOKE,
    DRIPPING,
    FLUSH,
    ERROR
};

struct Transition
{
    STATES next_state;
    std::function<bool()> condition;
};

struct State
{
    STATES state;
    Transition transitions[5];
    std::function<esp_err_t()> action;
};

// class State2{
// private:

// esp_err_t _status = ESP_OK;
// STATES _state;
// std::function<esp_err_t()> _action;
// // std::map<STATES, std::function<bool()>> _transitions;
// Transition _transitions[];

// public:

// State(STATES state, std::function<esp_err_t()> action, std::map<STATES, std::function<bool()>> transitions);

// STATES get_state(){return _state;}
// esp_err_t get_status(){return _status;}

// STATES check_transitions();
// esp_err_t execute();
// };