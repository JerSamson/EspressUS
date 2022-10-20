#include <Arduino.h>
#include "./main.h"
#include <ESP32CAN.h>
#include <CAN_config.h>

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
  while(millis() - startMillis < 5000) {
    can_t.send_CAN(100.0, 3.2, 80.0, 0, 1);
    ESP32Can.CANWriteFrame(&can_t.send_frame);
    delay(100);
  };

  startMillis = millis();
  Serial.println("Position #2");
  while(millis() - startMillis < 5000) {
    can_t.send_CAN(20.0, 3.2, 80.0, 0, 1);
    ESP32Can.CANWriteFrame(&can_t.send_frame);
    delay(100);
  };
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

