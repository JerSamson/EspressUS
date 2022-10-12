#include <Arduino.h>
#include "./main.h"

static Main my_main;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...\n");

  if(ESP_OK != my_main.setup()){
    Serial.print("ERROR - Main setup failed. Loop should not be executed.");
  }
}

void loop() {
  my_main.run();
  delay(200);
  // vTaskDelay(pdSECOND);
}

void scan_for_I2C()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...\n");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x\n");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x\n");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}


void Main::run(void)
{
  Serial.println("In main::run()...");

  if(!setup_success){
    Serial.println("WARNING - Cannot run main loop since main::setup() has not succeeded yet.");
    return;
  }

  // ==== Controller ==== 
  // controller.execute();

  // ==== LoadCell ==== 
  // Devices.loadCell.test_read();

  // ==== Thermocouple ==== 
  // Devices.thermocouple.test_read();

  // ==== LCD ==== 
  // scan_for_I2C();
  // Devices.lcd.test_display();

  // ==== Pressure Sensor ==== 
  // Devices.pressureSensor.get_pressure();
}

esp_err_t Main::setup()
{
  esp_err_t status {ESP_OK};

  Serial.println("===== Main Setup. =====");

  Devices = _Devices::getInstance();

  // ===== LoadCell ===== 
  status |= Devices.loadCell.setup(true);
  if(ESP_OK != status){
    Serial.println("ERROR - Main::Setup() - Loadcell setup failed.");
    return status;
  }

  // ===== Thermocouple ===== 
  // No init needed

  // ===== LCD ===== 
  status |= Devices.lcd.setup(true);
  if(ESP_OK != status){
    Serial.println("ERROR - Main::Setup() - LCD setup failed.");
    return status;
  }

  // ===== Pressure Sensor =====
  status |= Devices.pressureSensor.init();
  if(ESP_OK != status){
    Serial.println("ERROR - Main::Setup() - Failed to init pressure sensor's adc.");
    return status;
  }

  controller.clear_history();

  // Old stuff
  // status |= pressureSensor.init();
  // status |= analogReadTestPin.init();

  // analogReadTestPin.set(true);

  // ble_pressure.add_characteristic("Test", BLECharacteristic::PROPERTY_READ);
  // ble_pressure.update_characteristic("Test", "this is a test value");

  //Adafruit_GrayOLED LED(1, 128, 64, twi_p, -1, 400000, 100000);

  // LED = Adafruit_GrayOLED(1, 64, 128, twi_p, -1, 400000, 100000);
  setup_success = ESP_OK == status;
  return status;
}
