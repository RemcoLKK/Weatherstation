#include <Wire.h>
#include <oLed.hpp>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oLedSetup() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }

  display.display();
  delay(2000); 

  display.setTextSize(2);  
  display.setTextColor(WHITE); 
}