#include <Arduino.h>
#include "MAX31855K_test.h"
#include "HX711_test.h"

#include <chrono>
#include <map>

class Controller{
    private:
    max31855k_t thermocouple;
    hx711_t loadCell;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    float load = 0;
    float temp = 0;

    std::map<uint64_t, float> temp_history;
    std::map<uint64_t, float> load_history;
    
    public:

    void reset_time();
    int64_t get_ellapsed();
    void update_values();

    Controller(max31855k_t, hx711_t);

};