#pragma once

#include <map>
#include <functional>
#include "Arduino.h"

enum STATES{
    IDLE         = 1,
    INIT         = 2,
    HEATING      = 3,
    FILL_BOILER  = 4,
    READY        = 5,
    VERIN_UP     = 6,
    FILLING_HEAD = 7,
    EXTRACT      = 8,
    DONE         = 9,
    CHOKE        = 10,
    DRIPPING     = 11,
    FLUSH        = 12,
    ERROR        = 0
};

struct Transition
{
    STATES next_state;
    bool (* condition) ();
};

struct State
{
    STATES state;
    Transition transitions[5];
    esp_err_t (* action) ();
};

// bool operator==(const State& lhs, const State& rhs)
// {
//     return lhs.state == rhs.state;
// }