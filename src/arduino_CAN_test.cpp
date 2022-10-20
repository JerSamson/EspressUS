#include "arduino_CAN_test.h"

void arduino_CAN_test::send_CAN(float targetPos, float currentLim, float dutyCycle, int mvtProfile, bool allowMvt)
{
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
}

void arduino_CAN_test::setup_frame_CAN()
{
    wake_frame.FIR.B.FF = CAN_frame_std;
    wake_frame.MsgID = 0x00;
    wake_frame.FIR.B.DLC = 2;
    wake_frame.data.u8[0] = 0x01;
    wake_frame.data.u8[1] = 0x00;
    
    send_frame.FIR.B.FF = CAN_frame_std;
    send_frame.MsgID = 0x21B;
    send_frame.FIR.B.DLC = 8;
}