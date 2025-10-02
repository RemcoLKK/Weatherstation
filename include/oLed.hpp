#ifndef OLED_HPP
#define OLED_HPP

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_SDA 13 
#define OLED_SCL 14

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

extern TwoWire I2C_OLED;
extern Adafruit_SSD1306 display;
void oLedSetup();

#endif // OLED_HPP