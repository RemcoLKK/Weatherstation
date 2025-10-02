#include <Arduino.h>
#include <uv.hpp>
#include <oLed.hpp>

int getUVLevel() {
  int uvValue = analogRead(UV_PIN);
  int uvLevel = 0;
  if (uvValue < 1200) {
    uvLevel = 0;
  } else if (uvValue <= 1433) {
    uvLevel = 1;
  } else if (uvValue <= 1666) {
    uvLevel = 2;
  } else if (uvValue <= 1900) {
    uvLevel = 3;
  } else if (uvValue <= 2133) {
    uvLevel = 4;
  } else if (uvValue <= 2366) {
    uvLevel = 5;
  } else if (uvValue <= 2600) {
    uvLevel = 6;
  } else if (uvValue <= 2833) {
    uvLevel = 7;
  } else if (uvValue <= 3066) {
    uvLevel = 8;
  } else if (uvValue <= 3300) {
    uvLevel = 9;
  } else {
    uvLevel = 10;
  }
  return uvLevel;
}

void printUVLevel() {
  display.clearDisplay(); 
  display.setCursor(0, 0);
  display.print(F("UV Level: "));   
  display.println(getUVLevel());
  display.display();
  delay(100);
}