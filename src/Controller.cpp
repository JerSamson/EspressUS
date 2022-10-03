#include "Controller.h"

Controller::Controller(max31855k_t* thermo, hx711_t* lc, sh110x_t* _lcd) : thermocouple(thermo), loadCell(lc), lcd(_lcd){
    Serial.println("Constructing controller...");
    clear_history();
}

STATES Controller::execute(){
    Serial.println("In Controller::Execute()");
    STATES next_state = current_state;
    // update_values();

    last_execution = std::chrono::steady_clock::now();
    
    const char* text_idle = "IDLE";
    const char* text_preinf = "PRE INFUSION";
    const char* text_inf = "INFUSION";
    const char* text_heat1 = "HEAT1";
    const char* text_heat2 = "HEAT2";

    switch (current_state)
    {
    case STATES::IDLE:
        Serial.println("Executing state IDLE...");
        lcd->display_text(text_idle);
        if(ms_since_start() > 10000){
            next_state = STATES::PRE_INFUSION;
            reset_time();
        }

        break;
    
    case STATES::PRE_INFUSION:
        Serial.println("Executing state PRE_INFUSION...");
        lcd->display_text(text_preinf);

        if(ms_since_start() > 10000){
            next_state = STATES::HEAT_1;
            reset_time();
        }
        break;

    case STATES::HEAT_1:
        Serial.println("Executing state HEAT_1...");
        lcd->display_text(text_heat1);

        if(ms_since_start() > 10000){
            next_state = STATES::HEAT_2;
            reset_time();
        }

        break;

    case STATES::HEAT_2:
        Serial.println("Executing state HEAT_2...");
        lcd->display_text(text_heat2);
    

        if(ms_since_start() > 10000){
            next_state = STATES::IDLE;
            reset_time();
        }
        break;

    case STATES::ERROR:
        Serial.println("Executing state ERROR...");
        // lcd->display_text("ERROR");
        break;

    default:
        Serial.println("WARNING - Controller in unknown state... Returning IDLE.");
        next_state = STATES::IDLE;
        break;
    }

    current_state = next_state;
}

void Controller::reset_time(){
    begin = std::chrono::steady_clock::now();
}

int64_t Controller::get_ellapsed_ms(std::chrono::steady_clock::time_point since){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - since).count();
}

void Controller::update_values(){

    uint64_t ellapsed = ms_since_start();

    // Temperature readings
    if(thermocouple->is_init()){
        float last_temp = temp;
        uint64_t last_ellapsed = (temp_history.rbegin())->first;

        // Get and save current temp
        temp = thermocouple->get_temp();

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
    if(loadCell->can_read()){
        float last_load = temp;
        uint64_t last_ellapsed = (load_history.rbegin())->first;

        // Get and save current load
        load = loadCell->get_load();
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
    i_temp_history[0] = 0;

    load_history.clear();
    d_load_history.clear();
    i_load_history.clear();
    i_load_history[0] = 0;

    Serial.println("CONTROLLER - Data history cleared.");
}