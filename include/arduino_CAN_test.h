#include <ESP32CAN.h>
#include <CAN_config.h>

class arduino_CAN_test
{
private:
public:
    CAN_frame_t wake_frame;
    CAN_frame_t send_frame;
    void send_CAN(float targetPos, float currentLim, float dutyCycle, int mvtProfile, bool allowMvt);
    void setup_frame_CAN();
};