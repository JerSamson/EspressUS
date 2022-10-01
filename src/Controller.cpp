#include "Controller.h"

Controller::Controller(max31855k_t* thermo, hx711_t* lc, sh110x_t* _lcd) : thermocouple(thermo), loadCell(lc), lcd(_lcd){
  Serial.println("Constructing controller...");
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

    if(thermocouple->is_init()){
        temp = thermocouple->get_temp();
        temp_history[ellapsed] = temp;
    }
    else{
        Serial.println("WARNING - Could not get temperature since thermocouple is not initialized.");
    }

    if(loadCell->is_init() && loadCell->is_active()){
        load = loadCell->get_load();
        load_history[ellapsed] = load;
    }
    else{
        Serial.println("WARNING - Could not get loadCell data since it is either not initialized or inactive.");
    }
}