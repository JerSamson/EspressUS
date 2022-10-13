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
