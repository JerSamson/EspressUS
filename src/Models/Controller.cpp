#include "./Models/Controller.h"

Controller::Controller(){
    Devices = _Devices::getInstance();
    clear_history();
}

State Controller::get_next_state() const{
    // if(ESP_OK != _status){
    //     return STATES::ERROR;
    // }

    for( unsigned int i = 0; i < sizeof(current_state.transitions)/sizeof(current_state.transitions[0]); i = i + 1 ){
        if(current_state.transitions[i].condition()){
            return state_map.at(current_state.transitions[i].next_state);
        }
    }

    return current_state;
}

esp_err_t Controller::execute(){
    esp_err_t status{ESP_OK};

    status |= current_state.action();

    if(ESP_OK == status){
        current_state = get_next_state();
    }

    return status;
}

void Controller::reset_time(){
    begin = std::chrono::steady_clock::now();
}

int64_t Controller::get_ellapsed_ms(std::chrono::steady_clock::time_point since){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - since).count();
}

#define GET_VARIABLE_NAME(Variable) (#Variable)
void Controller::debug_print_map(std::map<uint64_t, float> map){
    Serial.printf("%s:\n", GET_VARIABLE_NAME(map));
    std::map<uint64_t, float>::iterator it;
    for (it = map.begin(); it != map.end(); it++)
    {
        Serial.printf("\t%2f\n", it->second);
    }
}

void Controller::update_values(){

    uint64_t ellapsed = ms_since_start();

    // Temperature readings
    Serial.println("INFO - Controller::update_values() - Reading temperature...");
    if(using_thermo && Devices.thermocouple.is_init()){
        float last_temp = temp;
        uint64_t last_ellapsed = NAN;

        if(!temp_history.empty()){
            last_ellapsed = (temp_history.rbegin())->first;
        }

        // Get and save current temp
        temp = Devices.thermocouple.get_temp();

        // Valid reading
        if(temp != -1){
            temp_history[ellapsed] = temp;

            // Compute and save temp derivative according to last mesured temp
            if(last_temp != NAN){
                d_temp_history[ellapsed] = derivate(last_ellapsed, last_temp, ellapsed, temp);
            }

            // Compute and save temp integral according to last mesured temp
            float last_i = (i_temp_history.rbegin())->second;
            float i = integrate(last_ellapsed, last_temp, ellapsed, temp);
            i_temp_history[ellapsed] = last_i + i;
        }else{
            Serial.println("WARNING - Controller::update_values() - Could not get a valid reading from thermocouple.");
        }
    }
    else{
        Serial.println("WARNING - Controller::update_values() - Could not get temperature since thermocouple is not initialized.");
    }

    // Load cell readings
    Serial.println("INFO - Controller::update_values() - Reading Load cell...");
    if(using_loadcell && Devices.loadCell.can_read()){
        float last_load = temp;
        uint64_t last_ellapsed = NAN;

        if(!load_history.empty()){
            last_ellapsed = (load_history.rbegin())->first;
        }

        // Get and save current load
        load = Devices.loadCell.get_load();
        load_history[ellapsed] = load;

        // TODO: No direct way to validate reading...

        // Compute and save derivative according to last mesured load
        if(last_load != NAN){
            d_load_history[ellapsed] = derivate(last_ellapsed, last_load, ellapsed, load);
        }

        // Compute and save load integral according to last mesured load
        float last_i = (i_load_history.rbegin())->second;
        float i = integrate(last_ellapsed, last_load, ellapsed, load);
        i_load_history[ellapsed] = last_i + i;
    }
    else{
        Serial.println("WARNING - Controller::update_values() - Could not get loadCell data since it is either not initialized or inactive.");
    }

    // Pressure readings
    Serial.println("INFO - Controller::update_values() - Reading pressure...");
    if(using_pressure && Devices.pressureSensor.is_init()){
        float last_pressure = pressure;
        uint64_t last_ellapsed = NAN;
        
        if(!pressure_history.empty()){
            last_ellapsed = (pressure_history.rbegin())->first;
        }

        // Get and save current load
        pressure = Devices.pressureSensor.get_pressure();

        Serial.println("DEBUG - Saving pressure in pressure history map");
        pressure_history[ellapsed] = pressure;

        // TODO: No direct way to validate reading...

        // Compute and save derivative according to last mesured load
        Serial.println("DEBUG - Computing pressure derivative");
        if(last_pressure != NAN){
            d_pressure_history[ellapsed] = derivate(last_ellapsed, last_pressure, ellapsed, pressure);

            // debug_print_map(i_pressure_history);

            // Compute and save load integral according to last mesured load
            Serial.println("DEBUG - Computing pressure integral");
            Serial.printf("i_p_history size: %d\n", i_pressure_history.size());
            float last_i = (--i_pressure_history.end())->second;

            Serial.printf("last_i: %f\n", last_i);
            float i = integrate(last_ellapsed, last_pressure, ellapsed, pressure);
            Serial.printf("I: %f\n", i);

            Serial.printf("new_I: %f\n", (last_i + i));

            Serial.println("DEBUG - saving pressure integral");
            i_pressure_history[ellapsed] = last_i + i;
            Serial.println("DEBUG - pressure integral saved");
        }
    }
    else{
        Serial.println("WARNING - Controller::update_values() - Could not get pressure Sensor data since it is either not initialized or inactive.");
    }
}

float Controller::derivate(uint64_t time1, float value1, uint64_t time2, float value2, int time_factor){
    float dvalue = value2 - value1;
    uint64_t dtime = (time2 - time1)/time_factor;
    return dvalue/dtime;
}

float Controller::integrate(uint64_t time1, float value1, uint64_t time2, float value2, int time_factor){
    uint64_t dtime = (time2 - time1)/time_factor;
    if(value1 > value2){
        return (value2 * dtime) + ((value1 - value2)*dtime/2);
    }
    else if (value1 < value2)
    {
        return (value1 * dtime) + ((value2 - value1)*dtime/2);
    }
    else{
        return value1*dtime;
    }
}

// Clears and initialize history maps
void Controller::clear_history(){
    temp_history.clear();
    d_temp_history.clear();
    i_temp_history.clear();
    i_temp_history[0] = 0.0f;

    load_history.clear();
    d_load_history.clear();
    i_load_history.clear();
    i_load_history[0] = 0.0f;

    pressure_history.clear();
    d_pressure_history.clear();
    i_pressure_history.clear();
    i_pressure_history[0] = 0.0f;

    Serial.println("DEBUG - i_pressure_history after clear:");
    debug_print_map(i_pressure_history);

    Serial.println("CONTROLLER - Data history cleared.");
}

// ================== ACTIONS ==================

    esp_err_t Controller::idle_action(){
        Devices.lcd.display_text("IDLE");
        return ESP_OK;  
    }
    
    esp_err_t Controller::init_action(){
        Devices.lcd.display_text("INIT");
        return ESP_OK;  
    }
    
    esp_err_t Controller::heating_action(){
        Devices.lcd.display_text("HEATING");
        return ESP_OK;  
    }
    
    esp_err_t Controller::fill_boiler_action(){
        Devices.lcd.display_text("FILL BOILER");
        return ESP_OK;  
    }
    
    esp_err_t Controller::ready_action(){
        Devices.lcd.display_text("READY");
        return ESP_OK;  
    }
    
    esp_err_t Controller::verin_up_action(){
        Devices.lcd.display_text("VERIN UP");
        return ESP_OK;  
    }
    
    esp_err_t Controller::filling_head_action(){
        Devices.lcd.display_text("FILLING HEAD");
        return ESP_OK;  
    }
    
    esp_err_t Controller::extract_action(){
        Devices.lcd.display_text("EXTRACT");
        return ESP_OK;  
    }
    
    esp_err_t Controller::done_action(){
        Devices.lcd.display_text("DONE");
        return ESP_OK;  
    }
    
    esp_err_t Controller::choke_action(){
        Devices.lcd.display_text("CHOKE");
        return ESP_OK;  
    }
    
    esp_err_t Controller::dripping_action(){
        Devices.lcd.display_text("DRIPPING");
        return ESP_OK;  
    }
    
    esp_err_t Controller::flush_action(){
        Devices.lcd.display_text("FLUSH");
        return ESP_OK;  
    }
    
    esp_err_t Controller::error_action(){
        Devices.lcd.display_text("ERROR");
        return ESP_OK;  
    }


// ================== TRANSITIONS ==================

    bool Controller::idle_to_init(){
        // Water level check
        // Load cell memory is not full?
        return false;
    }

    bool Controller::init_to_heating(){
        // Position Verin = down
        return false;
    }

    bool Controller::heating_to_ready(){
        float target = 95.0;
        return Devices.thermocouple.get_temp() >= target;
    }

    bool Controller::heating_to_fill_boiler(){
        // Niveau d'eau trop bas 
        return false;
    }

    bool Controller::fill_boiler_to_heating(){
        // Niveau d'eau alright et user action
        return false;
    }

    bool Controller::ready_to_fill_boiler(){
        // Niveau d'eau trop bas 
        return false;
    }
    bool Controller::ready_to_verin_up(){
        // User action
        return false;
    }

    bool Controller::verin_up_to_fill_head(){
        // Position verin = up
        return false;
    }

    bool Controller::fill_head_to_extract(){
        return false;
    }

    bool Controller::extract_to_enjoy(){
        return false;
    }

    bool Controller::extract_to_choke(){
        return false;
    }

    bool Controller::enjoy_to_dripping(){
        return false;
    }

    bool Controller::choke_to_ready(){
        return false;
    }

    bool Controller::choke_to_flush(){
        return false;
    }

    bool Controller::dripping_to_ready(){

        return false;
    }

    bool Controller::dripping_to_flush(){
        return false;
    }

    bool Controller::flush_to_ready(){
        return false;
    }