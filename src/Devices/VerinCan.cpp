#include "./Devices/VerinCan.h"

CAN_device_t CAN_cfg;

esp_err_t VerinCan::init(){
    esp_err_t status{ESP_OK};
    Serial.println("Initializing CANOpen comm");

    //initialize CAN Module
    if(ESP32Can.CANInit() != 0){
        status = ESP_FAIL;
    }

    setup_frame_CAN();

    init_success = status == ESP_OK;
    return status;
}

esp_err_t VerinCan::wake_up(){
    esp_err_t status{ESP_OK};

    Serial.println("INFO\t- VerinCan::Wake_up() - Waking up the verin");
    status |= ESP32Can.CANWriteFrame(&wake_frame);
    last_wakeup_ms = std::chrono::high_resolution_clock::now();
    
    return status;
}

esp_err_t VerinCan::send_CAN(float targetPos, float dutyCycle, float currentLim,  int mvtProfile, bool allowMvt){
    if(targetPos > 112 || targetPos < 0){
        Serial.println("Warning - VerinCAN::to_CAN - Invalid target position");
        return ESP_FAIL;
    }
    esp_err_t status{ESP_OK};

    if(get_ellapsed_ms_d(last_wakeup_ms) >= 1800) {
        Serial.println("Debug\t- send_CAN - Auto Wake Up Verin");
        wake_up();
    }

    last_wakeup_ms = std::chrono::high_resolution_clock::now();

    status |= to_CAN(targetPos, currentLim, dutyCycle, mvtProfile, allowMvt);
    
    int nFramesWritten = ESP32Can.CANWriteFrame(&send_frame);
    
    Serial.printf("Debug\t- ESP32Can::CANWriteFrame() - returned %d\n", nFramesWritten);

    return status;
}

int VerinCan::receive_CAN(controlParam option){
    if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){
        switch(option){
            case position:
                return (rx_frame.data.u8[1] << 8) + rx_frame.data.u8[0];
                break;
            case vitesse:
                return (rx_frame.data.u8[5] << 8) + rx_frame.data.u8[4];
                break;
            case courant:
                return (rx_frame.data.u8[3] << 8) + rx_frame.data.u8[2];
                break;
            default:
                return (rx_frame.data.u8[1] << 8) + rx_frame.data.u8[0];
                break;
        }
        // return rx_frame.data.u8[0] | rx_frame.data.u8[1] << 8;
        // return (rx_frame.data.u8[1] << 8) + rx_frame.data.u8[0];
    }else{
        return -1;
    }
}

VerinCan::VerinCan(gpio_num_t rx, gpio_num_t tx){
    CAN_cfg.tx_pin_id = tx;
    CAN_cfg.rx_pin_id = rx;

    // start the CAN bus at 500 kbps
    CAN_cfg.speed = CAN_SPEED_500KBPS;

    /* create a queue for CAN receiving */
    CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
}

esp_err_t VerinCan::to_CAN(float targetPos, float currentLim, float dutyCycle, int mvtProfile, bool allowMvt)
{
    if(!init_success){
        Serial.println("ERROR\t- Send_CAN() - Init not done");
        return ESP_ERR_INVALID_STATE;
    }

    if(targetPos > 112 || targetPos < 0){
        Serial.println("Warning\t- VerinCAN::to_CAN - Invalid target position");
        return ESP_FAIL;
    }

    // Target Position in (mm), 0.0 - 150.0
    int pos_to_int = static_cast<int> (targetPos*10);
    send_frame.data.u8[0] = pos_to_int & 0xFF; //0xE8;
    send_frame.data.u8[1] = (pos_to_int >> 8) & 0xFF; //0x03;

    // Consumed Current Limit in (A), 0.0 - 4.0
    int cur_to_int = static_cast<int> (currentLim*10);
    send_frame.data.u8[2] = cur_to_int & 0xFF; //0x20;
    send_frame.data.u8[3] = (cur_to_int >> 8) & 0xFF; //0x00;

    // Target Speed in (%), 20.0 - 100.0
    int speed_to_int = static_cast<int> (dutyCycle*10);
    send_frame.data.u8[4] = speed_to_int & 0xFF; //0x20;
    send_frame.data.u8[5] = (speed_to_int >> 8) & 0xFF; //0x03;

    // Movement Profile, 0: Normal, 1: Precision, 2: Step
    send_frame.data.u8[6] = mvtProfile; //0x00;
    
    // Allow Movement (bool)
    send_frame.data.u8[7] = allowMvt; //0x01;
    return ESP_OK;
}

esp_err_t VerinCan::setup_frame_CAN()
{
    wake_frame.FIR.B.FF = CAN_frame_std;
    wake_frame.MsgID = 0x00;
    wake_frame.FIR.B.DLC = 2;
    wake_frame.data.u8[0] = 0x01;
    wake_frame.data.u8[1] = 0x00;
    send_frame.FIR.B.FF = CAN_frame_std;
    send_frame.MsgID = 0x21B;
    send_frame.FIR.B.DLC = 8;
    return ESP_OK;
}
