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
    WAIT_CLIENT  = 13,
    ERROR        = 0
};

const std::map<STATES, std::string> state2str_map = {
    {IDLE        , "Idle"}, 
    {INIT        , "Initialisation"},
    {HEATING     , "Heating"},
    {FILL_BOILER , "Fill boiler"},
    {READY       , "Ready"},
    {VERIN_UP    , "Verin up"},
    {FILLING_HEAD, "Filling head"},
    {EXTRACT     , "Extract"},
    {DONE        , "Done!"},
    {CHOKE       , "Choke"},
    {DRIPPING    , "Dripping"},
    {FLUSH       , "Flush"},
    {WAIT_CLIENT , "Waiting for client" },
    {ERROR       , "Error"} 
};

inline const char* ToString(STATES state)
{
    if(state2str_map.count(state) == 0) return "Unknown";

    return state2str_map.at(state).c_str();
}