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
    operation_mode = BLE::tryGetCharacteristic("OperationMode")->getValue() == "0" ? OperationMode::AUTO : OperationMode::MANUAL;

    // OperationMode::MANUAL
    if(operation_mode == MANUAL){
        set_state(MANUAL_STATE);
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
        else if(init_to_verin_up())  {set_state(VERIN_UP);}
        break;

    case VERIN_UP:
        // Actions
        status = verin_up_action();

        // Transitions
        if(ESP_OK != status)                {set_state(ERROR);}
        else if(verin_up_to_pre_infusion())    {set_state(PRE_INFUSION);}
        break;

    case PRE_INFUSION:
        // Actions
        status = pre_infusion_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(pre_infusion_to_infusion()) {set_state(INFUSION);}
        break;

    case INFUSION:
        // Actions
        status = infusion_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(infusion_to_enjoy())     {set_state(DONE);}
        break;

    case DONE:
        // Actions
        status = done_action();

        // Transitions
        if(ESP_OK != status)            {set_state(ERROR);}
        else if(enjoy_to_main_menu())    {set_state(WAIT_CLIENT);}
        break;

    case ERROR:
        Serial.printf("ERROR\t- Controller::execute() - In error state: (%s)\n", err_log);
        break;

    case MANUAL_STATE:
        status = manual_action();

        // Transitions
        if(ESP_OK != status)                            {set_state(ERROR);}
        else if(operation_mode == OperationMode::AUTO)  {
            set_state(WAIT_CLIENT);    
            last_state = WAIT_CLIENT; // Prevents code to resume Manual_State once connection is validated
        }
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
    if(state == ERROR){err_resolved=false;}
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

        float verin_target = verinUp? 112.0f : 0.0f;
        Devices.verin.send_CAN(verin_target, 20.0f, 3, 0, verinUp||verinDown);

        if(heating || flushing){
            Devices.testSSR.set(true);
        }else if(Devices.testSSR.get_state() == true){
            Devices.pump.stop();
            Devices.testSSR.set(false);
        }

        if(heating){
            Serial.println("INFO\t- Manual command received - HEATING");
            Devices.pump.send_command(100);
        }

        if(flushing){
            Serial.println("INFO\t- Manual command received - FLUSHING");
            Devices.pump.send_command(180);
        }

        if(verinUp||verinDown){
            Serial.println("INFO\t- Manual command received - VERIN UP/DOWN");
            Devices.verin.send_CAN(112.0f, 20.0f);   
        }
        
        // if(verinDown){
        //     Serial.println("INFO\t- Manual command received - VERIN DOWN");
        //     Devices.verin.send_CAN(0.0f, 20.0f);   
        // }

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
    
    esp_err_t Controller::verin_up_action(){
        print_once("DEBUG\t- VERIN UP ACTION FIRST LOOP");
        // Devices.lcd.display_text("VERIN UP");

        positionVerin = Devices.verin.receive_CAN(controlParam::position);
        if(positionVerin > 0x03E8){
            vitesseVerin = 30.0f;
        }else{
            vitesseVerin = 80.0f;
        }
        Devices.verin.send_CAN(112.0f, vitesseVerin);

        return status;
    }
    
    esp_err_t Controller::pre_infusion_action(){
        print_once("DEBUG\t- FILLING HEAD ACTION FIRST LOOP");
        // Devices.lcd.display_text("PRE-INFUSION");

        if(first_loop){
            Devices.pump.PIDPompe.setOutputBounded(true);
            Devices.pump.PIDPompe.setOutputBounds(-110.0, 110.0);
            Devices.pump.PIDPompe.setMaxIntegralCumulation(40.0);
            Devices.pump.PIDPompe.setTarget(1.5);
            Devices.pump.PIDPompe.setCumulStartFactor(0.8);

            Devices.testSSR.set(true);
            Devices.loadCell.calibrate(535.0);
            Devices.loadCell.zero();
            pumpRunning = true;
        }
        float pumpAdjust = Devices.pump.PIDPompe.tick(Devices.pressureSensor.read());

        Devices.pump.send_command((int) (110 + pumpAdjust));
        load = Devices.loadCell.read(1);

        return status;
    }
    
    esp_err_t Controller::infusion_action(){
        print_once("DEBUG\t- EXTRACT ACTION FIRST LOOP");
        // Devices.lcd.display_text("EXTRACT");

        if(first_loop){
            Devices.verin.PIDVerin.setOutputBounded(true);
            Devices.verin.PIDVerin.setOutputBounds(0.0, 75.0);
            Devices.verin.PIDVerin.setMaxIntegralCumulation(30.0);
            Devices.verin.PIDVerin.setTarget(8.5);
            Devices.verin.PIDVerin.setCumulStartFactor(0.5);
        }
        float speedAdjust = Devices.verin.PIDVerin.tick(Devices.pressureSensor.read());

        positionVerin = Devices.verin.receive_CAN(controlParam::position);
        if(positionVerin != -1){
            if(positionVerin < 0x00FA){
                speedAdjust = 0.0;
            }else if(pumpRunning && positionVerin < 0x03F2){
                Devices.pump.stop();
                pumpRunning = false;
            }
        }

        if(speedAdjust == 0.0 && Devices.pressureSensor.read() > 9.5){
            verinOn = false;
        }else{
            verinOn = true;
        }

        Devices.verin.send_CAN(0.0f, 20.0 + speedAdjust, 3.4, 0, verinOn);

        return status;
    }
    
    esp_err_t Controller::done_action(){
        print_once("DEBUG\t- DONE ACTION FIRST LOOP");
        // Devices.lcd.display_text("DONE");

        Devices.verin.send_CAN(20.0f, 80.0f);

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

    bool Controller::init_to_verin_up(){
        // Position Verin = down
        return false;
    }

    bool Controller::verin_up_to_pre_infusion(){
        if(positionVerin >= 0x0456)
            return true;
        return false;
    }

    bool Controller::pre_infusion_to_infusion(){
        return !(load < 5.0);
    }

    bool Controller::infusion_to_enjoy(){
        if(positionVerin != -1)
            if(positionVerin <= 0x0001)
                Devices.testSSR.set(false);
                return true;
        return false;
    }

    bool Controller::enjoy_to_main_menu(){
        if(positionVerin >= 0x00C3)
            return true;
        return false;
    }

    bool Controller::error_to_idle(){
        return false;
    }