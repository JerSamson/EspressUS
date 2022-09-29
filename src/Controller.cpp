#include "Controller.h"

Controller::Controller(max31855k_t thermo, hx711_t lc) : thermocouple(thermo), loadCell(lc){

}

void Controller::reset_time(){
    begin = std::chrono::steady_clock::now();
}

int64_t Controller::get_ellapsed(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count();
}

void Controller::update_values(){

    uint64_t ellapsed = get_ellapsed();

    temp = thermocouple.get_temp();
    load = loadCell.get_load();

    temp_history[ellapsed] = temp;
    load_history[ellapsed] = load;
}