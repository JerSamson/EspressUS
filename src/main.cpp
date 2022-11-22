#include "./main.h"

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

  // bool heating = BLE::tryGetCharacteristic("ManualHeat")->getValue() == "1";
  // bool flushing = BLE::tryGetCharacteristic("ManualFlush")->getValue() == "1";
  // bool verinUp = BLE::tryGetCharacteristic("ManualVerinUp")->getValue() == "1";
  // bool verinDown = BLE::tryGetCharacteristic("ManualVerinDown")->getValue() == "1";

  // if(heating){
  //   Serial.println("INFO\t- Manual command received - HEATING");
  // }
  // if(flushing){
  //   Serial.println("INFO\t- Manual command received - FLUSHING");
  // }
  // if(verinUp){
  //   Serial.println("INFO\t- Manual command received - VERIN UP");
  // }
  // if(verinDown){
  //   Serial.println("INFO\t- Manual command received - VERIN DOWN");
  // }
  
  // Add what to run here

}

esp_err_t Main::setup()
{
  esp_err_t status{ESP_OK};

  Serial.println("===== Main Setup. =====");

  Devices = _Devices::getInstance();
  status |= Devices.init();

  // Serial.println("DEBUG\t- Main::setup() - Loading configuration");

  // _Configuration& config = _Configuration::getInstance();
  // config.loadConfig(config_map.at(CONFIG::Default));

  controller.clear_history();

  setup_success = ESP_OK == status;

  Serial.println("\n========== End of Main::setup() ==========\n\n");

  return status;
}

// Tests methods

void Main::demo_tech(){
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
      // Devices.pressureSensor.get_pressure();
    }
    pos = Devices.verin.receive_CAN(rx_frame, controlParam::position);
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
  Serial.println("Pre-flush!");
  Devices.pump.send_command(105);
  delay(15000);
  Devices.pump.stop();

  //Insertion
  Serial.println("Insert ze filtre");
  delay(15000);

  // Pump x secondes
  Serial.println("Sending pump command...");
  int startPreInf = millis();
  Devices.pump.send_command(200);

  while(millis() - startPreInf < 15000){
    if(Devices.pressureSensor.read() > 2.0){
      break;
    }
    if(millis() - startPreInf > 8000){
      if(Devices.pressureSensor.read() == 0.0){
        while(true){
          Serial.println("PINE A LÈVE PAS CALISS");
        }
      }
    }
    delay(100);
  }

  Devices.pump.send_command(82);
  while(millis() - startPreInf < 15000){
    Devices.pressureSensor.read();
    delay(100);
  }

  Devices.pump.stop();
  Serial.println("Lowering verin...");

  Devices.verin.wake_up();
  auto time = std::chrono::high_resolution_clock::now();
  auto lastTime = time;
  float dt = 0.0;
  int on = true;
  float _p = 8.0;
  float _i = 0.0;
  float _d = 0.05;
  float cible = 6.5; //bars
  // float lastMeasure = 0.0; //bars
  float command; //Duty Cycle
  float error; //bars
  float lastError;
  float derivativeCycle;
  float integralCycle;
  float integralCumul = 0.0;
  float pressure;

  // Controller ctrl;
  Devices.pump.stop();
  while(true){

    pos = Devices.verin.receive_CAN(rx_frame, controlParam::position);
    pressure = Devices.pressureSensor.read();
    error = cible - pressure;
    time = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<double, std::milli>(time-lastTime).count();

    BLE::update_characteristic("Pressure", pressure);

    if(dt != 0.0){
      derivativeCycle = (error-lastError)/(dt/1000);
      integralCycle = ((lastError + error) / 2) * dt;
      integralCumul += integralCycle;
      if(derivativeCycle > 30.0){
        // command = error*p + integralCumul*_i;
        command = error*_p;
      }else{
        command = (error*_p + integralCumul*_i + derivativeCycle*_d);
        // command = (error*_p + derivativeCycle*_d);
        // printf("Command: %.2f ", command);
      }
    }else{
      command = error  * _p;
    }
    lastTime = time;
    lastError = error;
    if(pos < 0x00FA && pos != -1){
      command = 0.0;
    }else if(command < 0.0 && Devices.pressureSensor.read() > 9.5){
      on = false;
      command = 0.0;
    }else if(command < 0.0){
      command = 0.0;
    }else if(command > 95.0){
      command = 95.0;
    }else{
      on = true;
    }
    Devices.verin.send_CAN(0.0f, 20.0 + command, 3.2, 0, on);
    if(pos != -1 && pos <= 0x0001){
      Serial.println("U r done bro");
      break;
    }
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
  
  Serial.println("Execution done.");
  // delay(900000);
}

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

void Main::test_devices_freq()
{
  int i = 0;
  int j = 0;

  uint8_t pt_1;
  uint8_t pt_2;

  uint8_t delay;

  uint8_t min;
  uint8_t max;
  float avg;

  int itt = 10;
  int inner_itt = 100;

  min = 100;
  max = 0;
  avg = 0;

  for (i = 0; i < itt; i++)
  {
    pt_1 = millis();
    for (j = 0; j < inner_itt; j++)
      Devices.loadCell.read(10);
    pt_2 = millis();

    delay = pt_2 - pt_1;

    avg += delay / (itt * inner_itt);
    if (delay > max)
      max = delay;
    if (delay < min)
      min = delay;
  }

  Serial.println("Loadcell result:");
  Serial.printf("\tMin: %hhx\n", min);
  Serial.printf("\tMax: %hhx\n", max);
  Serial.printf("\tAvg: %f\n", avg);

  min = 100;
  max = 0;
  avg = 0;

  for (i = 0; i < itt; i++)
  {
    pt_1 = millis();
    for (j = 0; j < inner_itt; j++)
      Devices.pressureSensor.read();
    pt_2 = millis();

    delay = pt_2 - pt_1;

    avg += delay / (itt * inner_itt);
    if (delay > max)
      max = delay;
    if (delay < min)
      min = delay;
  }

  Serial.println("Pressure sensor result:");
  Serial.printf("\tMin: %hhx\n", min);
  Serial.printf("\tMax: %hhx\n", max);
  Serial.printf("\tAvg: %f\n", avg);

  // min = 100;
  // max = 0;
  // avg = 0;

  // for(i=0;i<itt;i++){
  //   pt_1 = millis();
  //   Devices.thermocouple.get_temp();
  //   pt_2 = millis();

  //   delay = pt_2 - pt_1;

  //   avg += delay/itt;
  //   if(delay > max)
  //     max = delay;
  //   if(delay < min)
  //     min = delay;
  // }

  // Serial.println("thermocouple result:");
  // Serial.printf("\tMin: %hhx\n", min);
  // Serial.printf("\tMax: %hhx\n", max);
  // Serial.printf("\tAvg: %f\n", avg);
}

void Main::demo_edika()
{

  Devices.lcd.display_logo();

  Serial.println("3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
  Serial.println("Here we go!");

  // Verin Up
  Devices.verin.wake_up();

  uint8_t ms = millis();
  while (millis() - ms < 20000)
  {
    Serial.println("Rising...");
    Devices.verin.send_CAN(112.0f);
    delay(100);
  }
  Serial.println("Verin raised!");

  Serial.println("Please insert porte-filter");
  delay(7500);

  // 3 way valve open (SSR)
  Serial.println("Opening 3 way valve...");
  Devices.testSSR.set(true);

  // Pump x secondes
  Serial.println("Sending pump command...");
  Devices.pump.send_command(150);
  delay(7250);
  Devices.pump.send_command(30);

  Serial.println("Lowering verin...");
  Devices.verin.wake_up();

  long lowering_start = millis();
  long now = lowering_start;

  // Verin Down / SSR OFF
  while (now - lowering_start < 25000)
  {
    Devices.verin.send_CAN(0.0f);

    if (now - lowering_start < 800 && Devices.testSSR.get_state() == true)
    {
      Serial.println("Shutting 3 way valve and pump");
      Devices.testSSR.set(false);
      Devices.pump.stop();
    }

    now = millis();
    delay(100);
  }

  // Verin up a bit
  long lowering_end = millis();
  now = lowering_end;
  Serial.println("Rising the verin a bit...");
  while (now - lowering_end < 2000)
  {

    Devices.verin.send_CAN(5.0f);

    now = millis();

    delay(100);
  }

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

  if (BLE::user_action_received(ua))
  {
    return true;
  }

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

void Main::test_ble_dynamic_plot()
{
  _floatCounter += 0.33;
  BLE::update_characteristic("Pressure", (float)log(_floatCounter));
  Serial.println(_floatCounter);
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

