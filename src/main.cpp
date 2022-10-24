#include <Arduino.h>
#include "./main.h"
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <iostream>
#include <iomanip>

CAN_device_t CAN_cfg;

static Main my_main;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...\n");
  my_main.setup();
}

uint32_t startMillis;
void loop() {
  my_main.run();
  vTaskDelay(pdSECOND);
}


void Main::run(void)
{
  Serial.print("In main::run()...\n");
  
  CAN_frame_t rx_frame;
  // CAN Bus
  startMillis = millis();
  Serial.println("Wake UP");
  while(millis() - startMillis < 500) {
    // Spam le reveil
    ESP32Can.CANWriteFrame(&can_t.wake_frame);
    delay(100);
  };

  startMillis = millis();
  Serial.println("Position #1");
  while(millis() - startMillis < 25000) {
    can_t.send_CAN(112.0, 1.5, 30.0, 0, 1);
    ESP32Can.CANWriteFrame(&can_t.send_frame);
    if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE)
    //do stuff!
      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("JMONTE OSTI LOOOOL");
      else
        printf("New extended frame");

      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          std::cout << std::hex << std::setfill('0') << std::setw(2) << +rx_frame.data.u8[i] << std::endl;
        }
        printf("\n");
      }
    delay(100);
  };

  // startMillis = millis();
  // Serial.println("Position #2");
  // while(millis() - startMillis < 12000) {
  //   can_t.send_CAN(30.0, 1.5, 50.0, 0, 1);
  //   ESP32Can.CANWriteFrame(&can_t.send_frame);
  //   delay(100);
  // };

  startMillis = millis();
  Serial.println("Position #2");
  while(millis() - startMillis < 30000) {
    can_t.send_CAN(0.0, 1.5, 20.0, 0, 1);
    ESP32Can.CANWriteFrame(&can_t.send_frame);
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE)
    //do stuff!
      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("PI LO JDESCENDS");
      else
        printf("New extended frame");

      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          std::cout << std::hex << std::setfill('0') << std::setw(2) << +rx_frame.data.u8[i] << std::endl;
        }
        printf("\n");
      }
    };
    delay(100);
}

esp_err_t Main::setup()
{
  esp_err_t status {ESP_OK};

  Serial.println("Main Setup\n");
  
  // ===== CANBus =====
  Serial.println("Initializing CANOpen comm");
  can_alim.init();
  can_alim.set(true);
  // start the CAN bus at 500 kbps
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = CAN_CTX_PIN;
  CAN_cfg.rx_pin_id = CAN_CRX_PIN;
  /* create a queue for CAN receiving */
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  //initialize CAN Module
  ESP32Can.CANInit();
  can_t.setup_frame_CAN();
  return status;
}

