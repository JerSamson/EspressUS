#pragma once

#include <map>
#include <functional>
#include "Arduino.h"

enum STATES{
    IDLE         = 1,
    INIT         = 2,
    VERIN_UP     = 3,
    PRE_INFUSION = 4,
    INFUSION     = 5,
    DONE         = 6,
    WAIT_CLIENT  = 7,
    MANUAL_STATE = 8,
    ERROR        = 0
};

const std::map<STATES, std::string> state2str_map = {
    {IDLE        , "Idle"}, 
    {INIT        , "Initialisation"},
    {VERIN_UP    , "Verin up"},
    {PRE_INFUSION, "Pre-Infusion"},
    {INFUSION    , "Infusion"},
    {DONE        , "Done!"},
    {WAIT_CLIENT , "Waiting for client"},
    {MANUAL_STATE, "Manual"},
    {ERROR       , "Error"}
};

inline const char* ToString(STATES state)
{
    if(state2str_map.count(state) == 0) return "Unknown";

    return state2str_map.at(state).c_str();
}