#include <Arduino.h>
#include "./main.h"

static Main my_main;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...\n");
  my_main.setup();
}

void loop() {
  my_main.run();
  vTaskDelay(pdSECOND);
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
  Serial.print("In main::run()...\n");

  // LoadCell
  // loadCell.test_read();

  // Thermocouple
  // thermo.test_read();

  // LCD
  // scan_for_I2C();
  // lcd.test_display();
  
  lcd.display.begin();

  lcd.display.setTextSize(1);
  lcd.display.setTextColor(SH110X_WHITE);
  lcd.display.setCursor(32, 0);
  const char *text = "Hello World!";
  const char *text2 = "Do you want coffee?";
  lcd.display.clearDisplay();
  lcd.display.println(text);
  lcd.display.println(text2);
  lcd.display.display();
  delay(10000);
  lcd.display_logo();
  delay(100000);

  // // float pressure = read_pressure();
  // // ble_pressure.update_pressure_value(pressure);
  // if(!display.begin(0x3C, true)){ // Address 0x3D default
  //   Serial.println("Couldnt start lcd screen");
  // } 

  // display.setContrast (125); // dim display
 
  // Serial.println("In Display...\n");

  // display.display();
  // delay(2000);

  // // Clear the buffer.
  // display.clearDisplay();

  // // draw a single pixel
  // display.drawPixel(10, 10, SH110X_WHITE);
  // display.drawCircle(20, 20, 10, SH110X_WHITE);
  // // Show the display buffer on the hardware.
  // // NOTE: You _must_ call display after making any drawing commands
  // // to make them visible on the display hardware!
  // display.display();
  // delay(2000);
  // display.clearDisplay();
}

esp_err_t Main::setup()
{
  esp_err_t status {ESP_OK};

  Serial.println("Main Setup\n");
    
  // ===== LoadCell ===== 
  Serial.print("Initializing load cell...\n");
  loadCellAlim.init();
  loadCellAlim.set(true);
  loadCell.setup();

  // ===== Thermocouple ===== 
  // No init needed

  // ===== LCD ===== 
  Serial.print("Initializing lcd...\n");
  lcdAlim.init();
  lcdAlim.set(true);
  Wire.begin(21, 22);


  // Old stuff
  // status |= pressureSensor.init();
  // status |= analogReadTestPin.init();

  // analogReadTestPin.set(true);

  // ble_pressure.add_characteristic("Test", BLECharacteristic::PROPERTY_READ);
  // ble_pressure.update_characteristic("Test", "this is a test value");

  //Adafruit_GrayOLED LED(1, 128, 64, twi_p, -1, 400000, 100000);

  // LED = Adafruit_GrayOLED(1, 64, 128, twi_p, -1, 400000, 100000);
  return status;
}

float Main::read_pressure(void)
{
    // int raw_data = pressureSensor.get_raw();
    // float voltage = pressureSensor.raw_to_voltage(raw_data);
    // float pressure = pressureSensor.voltage_to_pressure(voltage);

    // Serial.printf("Raw: %d\t Voltage: %f\tpressure: %f\n", raw_data, voltage, pressure);

    // return pressure;
    return 0.0;
}

