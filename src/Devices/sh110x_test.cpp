#include "./Devices/sh110x_test.h"

sh110x_t::sh110x_t(int _sda, int _clk, gpio_num_t _alim_pin, bool _initial_state): sda(_sda), clk(_clk), alim(Gpio::GpioOutput(_alim_pin)), initial_state(_initial_state){ 
}

esp_err_t sh110x_t::init(){

  Serial.println("INFO - sh110x_t::init() - Initializing LCD...");

  esp_err_t status {ESP_OK};

  status |= alim.init();
  status |= alim.set(initial_state);

  if(ESP_OK != status){
    Serial.println("ERROR - sh110x_t::setup() - Could not initialize alimentation gpio.");
    init_success = false;
    return status;
  }

  status |= Wire.begin(sda, clk) ? ESP_OK : ESP_ERR_INVALID_STATE;

  if(ESP_OK != status){
    Serial.println("ERROR - sh110x_t::setup() - Could not execute wire.Begin(sda, clk).");
    init_success = false;
    return status;
  }

  // display.begin(0x3C, false);  // initialize with the I2C addr 0x3D (for the 128x64)

  init_success = true;
  return status;
}

void sh110x_t::display_logo(){
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::display_logo() - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }

  display.begin(0x3C, false);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();

  display.drawBitmap(0, 0, troll, 128, 64, 1);
//   display.drawBitmap(0, 0, logo_espressus_bmp, 128, 64, 1);
  display.display();

  delay(2000);

}

void sh110x_t::display_text(const char* text, int textSize, int textColor, int cursorX, int cursorY, bool clear_display){
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::display_text - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  display.begin(0x3C, false);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.setTextSize(textSize);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(cursorX, cursorY);
  if(clear_display){
    display.clearDisplay();
  }
  display.println(text);
  display.display();
}

void sh110x_t::test_display()   {

  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::test_display() - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  //display.setContrast (0); // dim display

  // Start OLED
//   display.begin(0, true); // we dont use the i2c address but we will reset!
  display.begin(0x3C, false);  // initialize with the I2C addr 0x3D (for the 128x64)


  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  display.drawBitmap(0, 0, logo_espressus_bmp, 128, 64, 1);
  delay(2000);
  return;

  // draw a single pixel
  display.drawPixel(10, 10, SH110X_WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw many lines
  testdrawline();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw rectangles
  testdrawrect();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw a SH110X_WHITE circle, 10 pixel radius
  display.fillCircle(display.width() / 2, display.height() / 2, 10, SH110X_WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  testdrawroundrect();
  delay(2000);
  display.clearDisplay();

  testfillroundrect();
  delay(2000);
  display.clearDisplay();

  testdrawtriangle();
  delay(2000);
  display.clearDisplay();

  testfilltriangle();
  delay(2000);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(2000);
  display.clearDisplay();


  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Failure is always an option");
  display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
  display.clearDisplay();

  // miniature bitmap display
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(1);

  // invert the display
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
  display.clearDisplay();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}

void sh110x_t::testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawbitmap - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SH110X_WHITE);
    }
    display.display();
    delay(200);

    // then erase it + move it
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SH110X_BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}

void sh110x_t::testdrawchar(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawchar - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
  delay(1);
}

void sh110x_t::testdrawcircle(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawcircle - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  for (int16_t i = 0; i < display.height(); i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
}

void sh110x_t::testfillrect(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testfillrect(void) - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  uint8_t color = 1;
  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // alternate colors
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, color % 2);
    display.display();
    delay(1);
    color++;
  }
}

void sh110x_t::testdrawtriangle(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawtriangle(void) - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  for (int16_t i = 0; i < min(display.width(), display.height()) / 2; i += 5) {
    display.drawTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, SH110X_WHITE);
    display.display();
    delay(1);
  }
}

void sh110x_t::testfilltriangle(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - h110x_t::testfilltriangle(void) - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  uint8_t color = SH110X_WHITE;
  for (int16_t i = min(display.width(), display.height()) / 2; i > 0; i -= 5) {
    display.fillTriangle(display.width() / 2, display.height() / 2 - i,
                         display.width() / 2 - i, display.height() / 2 + i,
                         display.width() / 2 + i, display.height() / 2 + i, SH110X_WHITE);
    if (color == SH110X_WHITE) color = SH110X_BLACK;
    else color = SH110X_WHITE;
    display.display();
    delay(1);
  }
}

void sh110x_t::testdrawroundrect(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawroundrect(void) - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SH110X_WHITE);
    display.display();
    delay(1);
  }
}

void sh110x_t::testfillroundrect(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testfillroundrect(void) - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  uint8_t color = SH110X_WHITE;
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, color);
    if (color == SH110X_WHITE) color = SH110X_BLACK;
    else color = SH110X_WHITE;
    display.display();
    delay(1);
  }
}

void sh110x_t::testdrawrect(void) {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawrect(void) - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SH110X_WHITE);
    display.display();
    delay(1);
  }
}

void sh110x_t::testdrawline() {
  if(!is_active() || !is_init()){
    Serial.println("WARNING - sh110x_t::testdrawline() - Tried to use LCD but it is either inactive or not initialized.");
    return;
  }
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, SH110X_WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SH110X_WHITE);
    display.display();
    delay(1);
  }
  delay(250);
}
