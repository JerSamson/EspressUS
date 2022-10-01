#pragma once

#include "Adafruit_SH110X.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "./bitmaps/bitmaps.h"
#include "./Models/Gpio/gpio.h"

#define OLED_MOSI     12
#define OLED_CLK      14
#define OLED_DC       13
#define OLED_CS       -1
#define OLED_RST      -1

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

class sh110x_t{

private:
int sda;
int clk;
Gpio::GpioOutput alim;

bool init = false;

public:
bool is_active(){return alim.state();}
bool toggle(){
    alim.toggle();
    return alim.state();
}

sh110x_t(int _sda, int _clk, gpio_num_t _alim_pin);
// Create the OLED display
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);

esp_err_t setup(bool initial_state=true);

bool is_init(){ return init;}

  // lcd.display.begin();

  // lcd.display.setTextSize(1);
//   lcd.display.setTextColor(SH110X_WHITE);
  // lcd.display.setCursor(32, 0);
  // const char *text = "Hello World!";
  // const char *text2 = "Do you want coffee?";
  // lcd.display.clearDisplay();
  // lcd.display.println(text);
  // lcd.display.println(text2);
  // lcd.display.display();
  // delay(10000);
  // lcd.display_logo();
  // delay(100000);
void display_text(const char* text, int textSize=1, int textColor=SH110X_WHITE, int cursorX=0, int cursorY=0, bool clear_display=true);

void display_logo();

void test_display();

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h);

void testdrawchar(void);

void testdrawcircle(void);

void testfillrect(void);

void testdrawtriangle(void);

void testfilltriangle(void);

void testdrawroundrect(void);

void testfillroundrect(void);

void testdrawrect(void);

void testdrawline();
};

