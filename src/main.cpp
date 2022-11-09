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


void Main::demo_edika(){

  // Devices.lcd.display_logo();

  Serial.println("3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
  Serial.println("Here we go!");

  // Verin Up
  CAN_frame_t rx_frame;
  Devices.verin.wake_up();
  delay(250);

  int pos;
  uint8_t ms = millis();
  while(true){
    if(millis() - ms > 1200){
      Serial.println("Rising...");
      Devices.verin.send_CAN(112.0f);
      ms = millis();
      Devices.pressureSensor.get_pressure();
    }
    pos = Devices.verin.receive_CAN(rx_frame);
    if(pos >= 0x0456){
      delay(200);
      break;
    }
  }
  Serial.println("Verin raised!");
  delay(10000);
  // 3 way valve open (SSR)
  Serial.println("Opening 3 way valve...");
  Devices.testSSR.set(true);

  // Pump x secondes
  Serial.println("Sending pump command...");
  Devices.pump.send_command(200);
  delay(4200);
  // Devices.testSSR.set(false);
  // Devices.pump.stop();
  Devices.pump.send_command(30);

  Serial.println("Lowering verin...");
  Devices.verin.wake_up();

  long lowering_start = millis();
  long now = lowering_start;
  double last_powaaa = 0.0f;
  double powaaa = 60.0f;
  int on = true;

  while(true){
    if(millis() - lowering_start > 1200 || last_powaaa != powaaa){
      if(Devices.testSSR.get_state() == true){
        Serial.println("Shutting 3 way valve and pump");
        Devices.testSSR.set(false);
        Devices.pump.stop();
      }
      Devices.verin.send_CAN(0.0f, powaaa, 3.5, 0, on);
      last_powaaa = powaaa;
      lowering_start = millis();
    }
    pos = Devices.verin.receive_CAN(rx_frame);
    if(Devices.pressureSensor.get_pressure() == 0.0){
      powaaa = 90.0f;
    }else if(Devices.pressureSensor.get_pressure() < 4.0){
      powaaa = 75.0f;
    }else if(Devices.pressureSensor.get_pressure() < 6.0){
      powaaa = 60.0f;
    }else if(Devices.pressureSensor.get_pressure() < 8.0){
      powaaa = 40.0f;
    }else if(Devices.pressureSensor.get_pressure() < 9.0){
      powaaa = 20.0f;
    }else if(Devices.pressureSensor.get_pressure() > 10.0){
      on = 0;
      powaaa = 20.0f;
    }
    if(pos != -1 && pos <= 0x0001){
      Serial.println("U r done bro");
      break;
    }
    // if(pos != -1){
    //   if(pos <= 0x02BC && pos > 0x00C8){
    //     printf("powa 40 Current pos: %d\n", pos);
    //     powaaa = 40.0f;
    //   }else if(pos <= 0x00C8 && pos > 0x0001){
    //     printf("powa 20 Current pos: %d\n", pos);
    //     powaaa = 20.0f;
    //   }else if(pos <= 0x0001){
    //     Serial.println("U r done bro");
    //     break;
    //   }
    // }
  }

  long lowering_end = millis();
  now = lowering_end;
  Serial.println("Rising the verin a bit...");
  while(now - lowering_end < 2000){
  
    Devices.verin.send_CAN(5.0f);
    
    now = millis();

    delay(100);
  }

  Serial.println("Execution done.");
  delay(900000);
}

// BACKUP
// Serial.println("3...");
//   delay(800);
//   Serial.println("2...");
//   delay(800);
//   Serial.println("1...");
//   delay(800);
//   Serial.println("0!!!!!!!!!!!!!!!!!!!!!");

//   // Verin Up
//   Serial.println("PUTTING THE VERIN UP");

//   Devices.verin.wake_up();

//   uint8_t ms = millis();
//   while(millis() - ms < 20000){
//     Serial.println("COCK");
//     Devices.verin.send_CAN(112.0f);
//     delay(100);
//   }
//   Serial.println("SUCKER!");

//   delay(7500);

//   // 3 way valve open (SSR)
//   Serial.println("OPENING LA VALVE 3 FACONS");

//   Devices.testSSR.set(true);

//   // Pump x secondes
//   Serial.println("PUMP PUMP PUMP PUMP PIUMP PUMP PUMP PUMP");

//   Devices.pump.send_command(150);
//   delay(8000);
//   // Devices.pump.stop();

//   // delay(100);

//   Devices.pump.send_command(30);

//   // Devices.pump.send_command(62.5);
//   // delay(2500);
//   // Devices.pump.stop();

//   // Verin Down / SSR OFF
//   Serial.println("VERIN DOWN WATCH OUT");

//   Devices.verin.wake_up();

//   // uint8_t ms2 = millis();
//   int i = 0;
//   while(true){
//     // Serial.printf("Pressure: %f Bar\n", Devices.pressureSensor.get_pressure());

//     Devices.verin.send_CAN(0.0f);

//     if(i > 8 && Devices.testSSR.get_state() == true){

//       Serial.println("SHUTTING OFF THE VALVE 3 FACONS");
//       Devices.testSSR.set(false);
//       Devices.pump.stop();

//     }

//     if(i++ > 200)
//       break;

//     delay(100);
//   }


//   i = 0;
//   while(true){
//     // Serial.printf("Pressure: %f Bar\n", Devices.pressureSensor.get_pressure());

//     Devices.verin.send_CAN(5.0f);

//     if(i++ > 20)
//       break;

//     delay(100);
//   }

//   Serial.println("THE END.");
//   delay(100000);


void Main::run(void)
{
  // Serial.println("In main::run()...");

  if(!setup_success){
    Serial.println("WARNING - Cannot run main loop since main::setup() has not succeeded yet.");
    return;
  }

  demo_edika();

  // delay(5000);

  // Serial.println("Setting SSR and 3 way ON");
  // Devices.testSSR.set(true);
  // delay(500);
  // Devices.pump.send_command(125);

  // delay(7000);

  // Serial.println("Setting SSR and 3 way OF");


  // Devices.pump.stop();
  // delay(500);
  // Devices.testSSR.set(false);

  // Serial.println("OVER");

  // delay(999999);

  // ==== Controller ==== 
  // controller.execute();

  // ==== LoadCell ==== 
  // Devices.loadCell.zero();
  // float l = Devices.loadCell.get_load();
  // Serial.println(l);
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
  status |= Devices.init();

  Serial.println("DEBUG - Main::setup() - Loading configuration");
  // _Configuration& config = _Configuration::getInstance();
  // config.loadConfig(config_map.at(CONFIG::Default));

  // ===== LoadCell ===== 
  // status |= Devices.loadCell.init();
  // if(ESP_OK != status){
  //   Serial.println("ERROR - Main::Setup() - Loadcell setup failed.");
  //   return status;
  // }

  // Devices.loadCell.set(true);
  // Devices.loadCell.calibrate(451.9166584);
  // Devices.loadCell.zero();

  // ===== Thermocouple ===== 
  // No init needed

  // ===== LCD ===== 
  // status |= Devices.lcd.init(true);
  // if(ESP_OK != status){
  //   Serial.println("ERROR - Main::Setup() - LCD setup failed.");
  //   return status;
  // }

  // ===== Pressure Sensor =====
  // status |= Devices.pressureSensor.init();
  // if(ESP_OK != status){
  //   Serial.println("ERROR - Main::Setup() - Failed to init pressure sensor's adc.");
  //   return status;
  // }

  // ===== Pump controller =====
  // status |= Devices.pump.init();
  // if(ESP_OK != status){
  //   Serial.println("ERROR - Main::Setup() - Failed to init pump.");
  //   return status;
  // }

  // status |= Devices.testSSR.init();
  // if(ESP_OK != status){
  //   Serial.println("ERROR - Main::Setup() - Failed to init SSR.");
  //   return status;
  // }

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
