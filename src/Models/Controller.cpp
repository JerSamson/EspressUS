#include "./Models/Controller.h"

_Devices& Controller::Devices = _Devices::getInstance();
_Configuration& Controller::Configuration = _Configuration::getInstance();

Controller::Controller(){
    clear_history();
}

esp_err_t Controller::execute(){
    if(!Configuration.is_valid()){
        Serial.println("ERROR\t- Controller::execute - Invalid configuration.");
        status = ESP_ERR_INVALID_STATE;
        return status;
    }

    // Get operation mode
    OperationMode lastOpMode = operation_mode;
    operation_mode = BLE::tryGetCharacteristic("OperationMode")->getValue() == "0" ? OperationMode::AUTO : OperationMode::MANUAL;

    if(operation_mode == OperationMode::AUTO && lastOpMode != operation_mode){
        // When going from manual to auto, reset state to start
        last_state = WAIT_CLIENT; // Prevents code to resume Manual_State once connection is validated
        set_state(WAIT_CLIENT);    
    }

    // OperationMode::MANUAL
    if(operation_mode == MANUAL){
        set_state(MANUAL_STATE);
        manual_action();
        return status;
    }


    // OperationMode::AUTO 
    if(first_loop){
        Serial.printf("INFO\t- Controller::execute() - current state: '%s'\n", ToString(current_state));
    }

    STATES entry_state = current_state;

    switch (current_state)
    {
    case WAIT_CLIENT:
        // Actions
        status = wait_client_action();

        // Transitions
        if(ESP_OK != status)               {set_state(ERROR);}
        else if(wait_client_transition())  {
            if(last_state != WAIT_CLIENT){  // Connection was lost during a state. (Resume)
                set_state(last_state);}
            else{                           // Initial connection established
                set_state(IDLE);}
        }
        break;

    case IDLE:
        // Actions
        status = idle_action();

        // Transitions
        if(ESP_OK != status)    {set_state(ERROR);}
        else if(idle_to_init()) {set_state(INIT);}
        break;

    case INIT:
        // Actions
        status = init_action();

        // Transitions
        if(ESP_OK != status)        {set_state(ERROR);}
        else if(init_to_heating())  {set_state(HEATING);}
        break;

    case HEATING:
        // Actions
        status = heating_action();

        // Transitions
        if(ESP_OK != status)                {set_state(ERROR);}
        else if(heating_to_fill_boiler())   {set_state(FILL_BOILER);}
        else if(heating_to_ready())         {set_state(READY);}
        break;
    
    case FILL_BOILER:
        // Actions
        status = fill_boiler_action();

        // Transitions
        if(ESP_OK != status)                {set_state(ERROR);}
        else if(fill_boiler_to_heating())   {set_state(HEATING);}
        break;

    case READY:
        // Actions
        status = ready_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(ready_to_fill_boiler()) {set_state(FILL_BOILER);}
        else if(ready_to_verin_up())    {set_state(VERIN_UP);}
        break;

    case VERIN_UP:
        // Actions
        status = verin_up_action();

        // Transitions
        if(ESP_OK != status)                {set_state(ERROR);}
        else if(verin_up_to_fill_head())    {set_state(FILLING_HEAD);}
        break;

    case FILLING_HEAD:
        // Actions
        status = filling_head_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(fill_head_to_extract()) {set_state(EXTRACT);}
        break;

    case EXTRACT:
        // Actions
        status = extract_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(extract_to_choke())     {set_state(CHOKE);}
        else if(extract_to_enjoy())     {set_state(DONE);}
        break;

    case DONE:
        // Actions
        status = done_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(enjoy_to_dripping())    {set_state(DRIPPING);}
        break;

    case CHOKE:
        // Actions
        status = choke_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(choke_to_flush())       {set_state(FLUSH);}
        else if(choke_to_ready())       {set_state(READY);}
        break;

    case DRIPPING:
        // Actions
        status = dripping_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(dripping_to_flush())    {set_state(FLUSH);}
        else if(dripping_to_ready())    {set_state(READY);}
        break;

    case FLUSH:
        // Actions
        status = flush_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(flush_to_ready())       {set_state(READY);}
        break;

    case ERROR:
        Serial.printf("ERROR\t- Controller::execute() - In error state: (%s)\n", err_log);
        break;

    default:
        int state_num = (static_cast<int>(current_state));
        sprintf(err_log, "Controller was in an unknown state (%d)", state_num);
        set_state(ERROR);
        break;
    }

    // Same state as start of execute()
    if(current_state == entry_state){
        first_loop = false;
    }

    // Connection to client lost
    if(current_state != WAIT_CLIENT && connection_lost()){
        set_state(WAIT_CLIENT);
    }

    // Error was resolved - handle next state based on previous state
    if(current_state == ERROR && err_resolved){
        status = ESP_OK;
        switch (last_state)
        {
            // TODO: Handle other cases
            default:
                set_state(WAIT_CLIENT);
                break;
        }
    }

    return status;
}

void Controller::set_state(STATES state){
    if(current_state != state){
        last_state = current_state;
        current_state = state;
        first_loop = true;
        BLE::update_characteristic("CurrentState", state);
        Serial.printf("INFO\t- Controller::set_state() - Switching from state '%s' to '%s'\n\n", ToString(last_state), ToString(current_state));
    }else{
        first_loop = false;
    }
}

bool Controller::connection_lost(){
    return BLE::n_client == 0;
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

bool Controller::update_value(Sensor sensor, float &current_value, std::map<uint64_t, float> &value_map, std::map<uint64_t, float> &d_map, std::map<uint64_t, float> &i_map){
    if(!sensor.is_init()){
        status = ESP_ERR_INVALID_STATE;
        sprintf(err_log, "Tried to read uninitialized sensor.");
        return false;
    }

    uint64_t ellapsed = ms_since_start();

    // Get time of last reading, if any
    uint64_t last_ellapsed = 0;
    if(!value_map.empty()){
        last_ellapsed = (value_map.rbegin())->first;
    }

    // Get and save current value
    float last_value = current_value;
    current_value = sensor.read();

    bool isValueValid = true; // TODO: Validate value according to sensor? Sensor base class method?
    if(!isValueValid){
        //TODO: get sensor names
        Serial.println("WARNING - Controller::update_value() - Could not get a valid reading from sensor.");
        sprintf(err_log, "Got an invalid sensor reading.");
        return false;
    }

    value_map[ellapsed] = current_value;

    // Compute and save derivative according to last value
    if(last_value != NAN){
        d_map[ellapsed] = derivate(last_ellapsed, last_value, ellapsed, current_value);
    }

    // Compute and save temp integral according to last value
    // float last_i = (i_map.rbegin())->second;
    float last_i = (--i_map.end())->second;
    float i = integrate(last_ellapsed, last_value, ellapsed, current_value);
    i_map[ellapsed] = last_i + i;

    return true;
}

bool Controller::update_temp(){
    bool success = update_value(Devices.thermocouple, temp, temp_history, d_temp_history, i_temp_history);
    if(success){
        BLE::update_characteristic("Temp", temp);
    }
    return success;
}
bool Controller::update_load(){
    bool success = update_value(Devices.loadCell, load, load_history, d_load_history, i_load_history);
    if(success){
        BLE::update_characteristic("Load", load);
    }
    return success;
}
bool Controller::update_pressure(){
    bool success = update_value(Devices.pressureSensor, pressure, pressure_history, d_pressure_history, i_pressure_history);
    if(success){
        BLE::update_characteristic("Pressure", pressure);
    }
    return success;
}

void Controller::print_once(std::string str){
    if(first_loop){
        Serial.println(str.c_str());
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

    Serial.println("DEBUG\t- i_pressure_history after clear:");
    debug_print_map(i_pressure_history);

    Serial.println("CONTROLLER - Data history cleared.");
}

// ================== ACTIONS ==================

    esp_err_t Controller::manual_action(){
        bool heating = BLE::tryGetCharacteristic("ManualHeat")->getValue() == "1";
        bool flushing = BLE::tryGetCharacteristic("ManualFlush")->getValue() == "1";
        bool verinUp = BLE::tryGetCharacteristic("ManualVerinUp")->getValue() == "1";
        bool verinDown = BLE::tryGetCharacteristic("ManualVerinDown")->getValue() == "1";

        if(heating){
            Serial.println("INFO\t- Manual command received - HEATING");
        }
        if(flushing){
            Serial.println("INFO\t- Manual command received - FLUSHING");
        }
        if(verinUp){
            Serial.println("INFO\t- Manual command received - VERIN UP");
        }
        if(verinDown){
            Serial.println("INFO\t- Manual command received - VERIN DOWN");
        }
        return status;
    }

    esp_err_t Controller::wait_client_action(){
        print_once("DEBUG\t- WAIT CLIENT ACTION FIRST LOOP");
        print_once("Waiting for at least one connected client...");
        return status;
    }

    esp_err_t Controller::idle_action(){
        print_once("DEBUG\t- IDLE ACTION FIRST LOOP");

        // Request user action to start app
        if(first_loop){
            if(!BLE::request_user_action("StartApp")){
                sprintf(err_log, "Could not request user action 'StartApp'. Check logs for more details");
                status = ESP_ERR_INVALID_ARG;
            }
        }

        return status;  
    }
    
    esp_err_t Controller::init_action(){
        print_once("DEBUG\t- INIT ACTION FIRST LOOP");
        return status;  
    }
    
    esp_err_t Controller::heating_action(){
        print_once("DEBUG\t- HEATING ACTION FIRST LOOP");

        // Activate boiler SSR

        return status;  
    }
    
    esp_err_t Controller::fill_boiler_action(){
        // Devices.lcd.display_text("FILL BOILER");
        print_once("DEBUG\t- FILL BOILER ACTION FIRST LOOP");

        return status;  
    }
    
    esp_err_t Controller::ready_action(){
        print_once("DEBUG\t- READY ACTION FIRST LOOP");
        // Devices.lcd.display_text("READY");
        return status;  
    }
    
    esp_err_t Controller::verin_up_action(){
        print_once("DEBUG\t- VERIN UP ACTION FIRST LOOP");
        // Devices.lcd.display_text("VERIN UP");
        return status;  
    }
    
    esp_err_t Controller::filling_head_action(){
        print_once("DEBUG\t- FILLING HEAD ACTION FIRST LOOP");
        // Devices.lcd.display_text("FILLING HEAD");
        return status;  
    }
    
    esp_err_t Controller::extract_action(){
        print_once("DEBUG\t- EXTRACT ACTION FIRST LOOP");
        // Devices.lcd.display_text("EXTRACT");
        return status;  
    }
    
    esp_err_t Controller::done_action(){
        print_once("DEBUG\t- DONE ACTION FIRST LOOP");
        // Devices.lcd.display_text("DONE");
        return status;  
    }
    
    esp_err_t Controller::choke_action(){
        print_once("DEBUG\t- CHOKE ACTION FIRST LOOP");
        // Devices.lcd.display_text("CHOKE");
        return status;  
    }
    
    esp_err_t Controller::dripping_action(){
        print_once("DEBUG\t- DRIPPING ACTION FIRST LOOP");
        // Devices.lcd.display_text("DRIPPING");
        return status;  
    }
    
    esp_err_t Controller::flush_action(){
        print_once("DEBUG\t- FLUSH ACTION FIRST LOOP");
        // Devices.lcd.display_text("FLUSH");
        return status;  
    }
    
    esp_err_t Controller::error_action(){
        print_once("DEBUG\t- ERROR ACTION FIRST LOOP");
        // Devices.lcd.display_text("ERROR");
        return status;  
    }


// ================== TRANSITIONS ==================

    bool Controller::wait_client_transition(){
        return BLE::n_client() > 0;
    }

    bool Controller::idle_to_init(){
        std::string result;
        return BLE::try_get_user_action_result("StartApp", result);
    }

    bool Controller::init_to_heating(){
        // Position Verin = down
        return false;
    }

    bool Controller::heating_to_ready(){
        return Devices.thermocouple.read() >= Configuration.target_temp;
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

    bool Controller::error_to_idle(){
        return false;
    }