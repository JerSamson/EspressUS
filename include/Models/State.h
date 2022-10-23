#pragma once

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
    bool (* condition) ();
    // std::function<bool()> condition;
};

struct State
{
    STATES state;
    Transition transitions[5];
    esp_err_t (* action) ();
    // esp_err_t(*action)();
    // std::function<esp_err_t()> action;
};

// bool operator==(const State& lhs, const State& rhs)
// {
//     return lhs.state == rhs.state;
// }