#include <Arduino.h>
#include "./main.h"
#include "./Models/PID.h"

static Main my_main;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...\n");

  BLEDevice::init(DEVICE_NAME);
  BLE::setup();

  if (ESP_OK != my_main.setup())
  {
    Serial.print("ERROR\t- Main setup failed. Loop should not be executed.");
  }
}

void loop()
{
  my_main.run();
  delay(200);
}

void Main::run(void)
{
  if (!setup_success)
  {
    Serial.println("WARNING - Cannot run main loop since main::setup() has not succeeded yet.");
    return;
  }

  controller.execute();
  
  // Add what to run here

}

esp_err_t Main::setup()
{
  esp_err_t status{ESP_OK};

  Serial.println("===== Main Setup. =====");

  Devices = _Devices::getInstance();
  status |= Devices.init();

  Serial.println("DEBUG\t- Main::setup() - Loading configuration");
  _Configuration& config = _Configuration::getInstance();
  config.loadConfig(config_map.at(CONFIG::Default));

  controller.clear_history();

  setup_success = ESP_OK == status;

  Serial.println("\n========== End of Main::setup() ==========\n\n");

  return status;
}

// Tests methods

void scan_for_I2C()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...\n");
  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x\n");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x\n");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("done\n");
  }
  delay(5000);
}

void Main::demo_edika()
{

  // Devices.lcd.display_logo();

  Serial.println("3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
  Serial.println("Here we go!");

  // Verin Up
  // Devices.verin.wake_up();
  // delay(250);

  int pos;
  float speed = 80.0f;
  uint8_t ms = millis();
  while(true){
    if(millis() - ms > 1200){
      Serial.println("Rising...");
      Devices.verin.send_CAN(112.0f, speed);
      ms = millis();
    }
    pos = Devices.verin.receive_CAN(controlParam::position);
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
  while(millis() - startPreInf < 13000){
  
    float pumpAdjust = PIDPompe.tick(Devices.pressureSensor.read());
    printf("Factor: %.2f \n", pumpAdjust);

    Devices.pump.send_command((int) (110 + pumpAdjust));
    // Devices.loadCell.get_load(10);
  }
  // Devices.pump.stop();
  // delay(999999);

  Serial.println("Lowering verin...");
  // Devices.verin.wake_up();

  // std::chrono::_V2::system_clock::time_point time = std::chrono::high_resolution_clock::now();
  // auto lastTime = time;
  // float dt = 0.0;
  // int on = true;
  // float _p = 8.0;
  // float _i = 0.006;
  // float _d = 0.06;
  // float cible = 8.5; //bars
  // float command; //Duty Cycle
  // float error; //bars
  // float lastError;
  // float derivativeCycle;
  // float integralCycle;
  // float integralCumul = 0.0;
  // float pressure;
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

  bool IsOn = true;
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
    
    float speedAdjust = PIDVerin.tick(Devices.pressureSensor.read());

    pos = Devices.verin.receive_CAN(controlParam::position);
    if(pos != -1){
      if(pos <= 0x0001){
        Serial.println("U r done bro");
        break;
      }else if(pos < 0x00FA){
        speedAdjust = 0.0;
      }else if(pos < 0x03F2){
        Devices.pump.stop();
      }
    }

    if(speedAdjust == 0.0 && Devices.pressureSensor.read() > 9.5){
      IsOn = false;
    }else{
      IsOn = true;
    }

    Devices.verin.send_CAN(0.0f, 20.0 + speedAdjust, 3.4, 0, IsOn);
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
  Serial.printf("Ending weight: %.2f", Devices.loadCell.read(10));

  Serial.println("Execution done.");
  // delay(900000);
}

int _counter = 0;
void Main::test_ble()
{
  BLE::update_characteristic("Test", _counter++);
  Serial.println(_counter);
}

void Main::test_user_action()
{
  std::string ua = "StartApp";
  std::string result = "";

  if (!BLE::user_action_requested(ua))
    BLE::request_user_action(ua);

  Serial.printf("User action requested: %s\n", BLE::user_action_requested(ua) ? "YAS QUEEN" : "NAH FAM");
  bool received = BLE::try_get_user_action_result(ua, result);

  if (received)
  {
    Serial.printf("Value received: %s\n", result.c_str());
  }
  else
  {
    Serial.println("Waiting for user action...");
  }
}

bool Main::wait_for_user_action(std::string ua)
{
  // std::string ua = "StartApp";
  std::string result = "";

//   Devices.testSSR.set(true);

  if (!BLE::user_action_requested(ua))
    BLE::request_user_action(ua);

  return false;
}

float _floatCounter = 0.0f;
void Main::test_ble_float()
{
  _floatCounter += 0.33;
  BLE::update_characteristic("TestFloat", (float)sin(_floatCounter));
  Serial.println(_floatCounter);
}

int state_counter = 0;
int state_counter2 = 0;
void Main::test_ble_state()
{
  if (state_counter2 % 3 == 0)
  {
    state_counter++;
  }
  state_counter2++;

  BLE::update_characteristic("CurrentState", state_counter);

  if (state_counter > 12)
    state_counter = 0;

  Serial.println(state_counter);
}


float _loadCounter = 0.0f;
void Main::test_ble_load()
{
  _loadCounter += 0.33;
  if (_loadCounter >= 100)
    _loadCounter = 0;
  BLE::update_characteristic("Load", _loadCounter);
  Serial.println(_loadCounter);
}

float _tempCounter = 0.0f;
void Main::test_ble_temp()
{
  _tempCounter += 0.33;
  if (_tempCounter >= 100)
    _tempCounter = 0;
  BLE::update_characteristic("Load", _tempCounter);
  Serial.println(_tempCounter);
}

