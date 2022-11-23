#include <Arduino.h>
#include <chrono>
#include "./main.h"
#include "./Models/PID.h"

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
  float speed = 80.0f;
  uint8_t ms = millis();
  while(true){
    if(millis() - ms > 1200){
      Serial.println("Rising...");
      Devices.verin.send_CAN(112.0f, speed);
      ms = millis();
    }
    pos = Devices.verin.receive_CAN(rx_frame, controlParam::position);
    if(pos > 0x03E8){
      speed = 30.0f;
    }
    if(pos >= 0x0456){
      delay(200);
      break;
    }
  }
  Serial.println("Verin raised!");
  // 3 way valve open (SSR)
  Serial.println("Opening 3 way valve...");
  Devices.testSSR.set(true);

  //Pré-flush
  // Serial.println("Pre-flush!");
  // Devices.pump.send_command(105);
  // delay(15000);
  // Devices.pump.stop();
  
  //Insertion
  // Serial.println("Insert ze filtre");
  // delay(15000);
  
  // Pump x secondes
  Serial.println("Sending pump command...");
  // Devices.loadCell.calibrate(535.0);
  // Devices.loadCell.zero();
  // int startPreInf = millis();
  
  // Devices.pump.send_command(200);
  // while(true){
  //   if(Devices.pressureSensor.get_pressure() > 2.0){
  //     break;
  //   }
  //   if(millis() - startPreInf > 8000){
  //     if(Devices.pressureSensor.get_pressure() == 0.0){
  //       Devices.pump.stop();
  //       while(true){
  //         Serial.println("PINE A LÈVE PAS CALISS");
  //       }
  //     }
  //   }
  //   delay(100);
  // }
  // Devices.loadCell.zero();
  // Devices.loadCell.get_load(10);
  int startPreInf = millis();
  // Devices.pump.send_command(82);
  // float load = Devices.loadCell.get_load(10);
  // while(load < 5.0){
  //   Serial.printf("Pre-inf weight: %.2f", load);
  //   load = Devices.loadCell.get_load(10);
  //   // Devices.pressureSensor.get_pressure();
  //   delay(100);
  // }
  // Serial.printf("Pre-inf weight: %.2f", Devices.loadCell.get_load(10));
  float p_pompe = 70.0;
  float i_pompe = 14.0;
  float d_pompe = 0.0;
  PIDController<float> PIDPompe(p_pompe, i_pompe, d_pompe);
  PIDPompe.setOutputBounded(true);
  PIDPompe.setOutputBounds(-110.0, 110.0);
  PIDPompe.setMaxIntegralCumulation(40.0);
  PIDPompe.setTarget(1.5);
  PIDPompe.setCumulStartFactor(0.8);
  while(millis() - startPreInf < 18000){
  
    float pumpAdjust = PIDPompe.tick(Devices.pressureSensor.get_pressure());
    printf("Factor: %.2f \n", pumpAdjust);

    Devices.pump.send_command((int) (110 + pumpAdjust));
    // Devices.loadCell.get_load(10);
  }
  Devices.pump.stop();
  // Devices.testSSR.set(false);
  delay(999999);

  Serial.println("Lowering verin...");
  Devices.verin.wake_up();

  std::chrono::_V2::system_clock::time_point time = std::chrono::high_resolution_clock::now();
  auto lastTime = time;
  float dt = 0.0;
  int on = true;
  float _p = 8.0;
  float _i = 0.006;
  float _d = 0.06;
  float cible = 8.5; //bars
  float command; //Duty Cycle
  float error; //bars
  float lastError;
  float derivativeCycle;
  float integralCycle;
  float integralCumul = 0.0;
  float pressure;
  // Devices.pump.stop();
  // while(true){

  //   pos = Devices.verin.receive_CAN(rx_frame, controlParam::position);
  //   if(pos < 0x03F2){
  //     Devices.pump.stop();
  //   }
  //   pressure = Devices.pressureSensor.get_pressure();
  //   error = cible - pressure;
  //   time = std::chrono::high_resolution_clock::now();

  //   dt = std::chrono::duration<double, std::milli>(time-lastTime).count();
  //   if(dt != 0.0){
  //     derivativeCycle = (error-lastError)/(dt/1000);
  //     if(error < 0.6*cible){
  //       integralCycle = ((lastError + error) / 2) * dt;
  //       integralCumul += integralCycle;
  //     }
  //     if(derivativeCycle > 30.0){
  //       command = error*_p + integralCumul*_i;
  //       // command = error*_p;
  //     }else{
  //       command = (error*_p + integralCumul*_i + derivativeCycle*_d);
  //       // command = (error*_p + derivativeCycle*_d);
  //       // printf("Command: %.2f ", command);
  //     }
  //   }else{
  //     command = error  * _p;
  //   }
  //   lastTime = time;
  //   lastError = error;

  //   if(pos < 0x00FA && pos != -1){
  //     command = 0.0;
  //   }else if(command < 0.0 && Devices.pressureSensor.get_pressure() > 9.5){
  //     on = false;
  //     command = 0.0;
  //   }else if(command < 0.0){
  //     command = 0.0;
  //   }else if(command > 95.0){
  //     command = 95.0;
  //   }else{
  //     on = true;
  //   }

  //   Devices.verin.send_CAN(0.0f, 20.0 + command, 3.4, 0, on);

  //   if(pos != -1 && pos <= 0x0001){
  //     Serial.println("U r done bro");
  //     break;
  //   }
  // }
  bool on = true;
  float p_verin = 8.0;
  float i_verin = 0.006;
  float d_verin = 0.06;
  PIDController<float> PIDVerin(p_verin, i_verin, d_verin);
  PIDVerin.setOutputBounded(true);
  PIDVerin.setOutputBounds(0.0, 75.0);
  PIDVerin.setMaxIntegralCumulation(30.0);
  PIDVerin.setTarget(8.5);
  PIDVerin.setCumulStartFactor(0.5);
  while(true){
    
    pos = Devices.verin.receive_CAN(rx_frame, controlParam::position);
    if(pos != -1){
      if(pos <= 0x0001){
        Serial.println("U r done bro");
        break;
      }else if(pos < 0x00FA){
        command = 0.0;
      }else if(pos < 0x03F2){
        Devices.pump.stop();
      }
    }
    
    float speedAdjust = PIDVerin.tick(Devices.pressureSensor.get_pressure());

    if(speedAdjust == 0.0 && Devices.pressureSensor.get_pressure() > 9.5){
      on = false;
    }else{
      on = true;
    }

    Devices.verin.send_CAN(0.0f, 20.0 + speedAdjust, 3.4, 0, on);
  }
  Devices.testSSR.set(false);
  long lowering_end = millis();
  long now = lowering_end;
  Serial.println("Rising the verin a bit...");
  while(now - lowering_end < 2000){
  
    Devices.verin.send_CAN(20.0f, 80.0f);
    
    now = millis();

    delay(100);
  }
  delay(2000);
  Serial.printf("Ending weight: %.2f", Devices.loadCell.get_load(10));

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

  // Devices.pressureSensor.get_pressure();
  // delay(500);

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
