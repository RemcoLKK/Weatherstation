#include <Arduino.h>
#include <uv.hpp>
#include <oLed.hpp>

int getUVLevel() {
  int uvValue = analogRead(UV_PIN);
  int uvLevel = 0;
  if (uvValue < 1833) {
    uvLevel = 0;
  } else if (uvValue <= 2066) {
    uvLevel = 1;
  } else if (uvValue <= 2300) {
    uvLevel = 2;
  } else if (uvValue <= 2533) {
    uvLevel = 3;
  } else if (uvValue <= 2766) {
    uvLevel = 4;
  } else if (uvValue <= 3000) {
    uvLevel = 5;
  } else if (uvValue <= 3233) {
    uvLevel = 6;
  } else if (uvValue <= 3566) {
    uvLevel = 7;
  } else if (uvValue <= 3800) {
    uvLevel = 8;
  } else if (uvValue <= 4033) { // might not be accurate on values this high, but this value is maybe reached once in 10 years in the Netherlands
    uvLevel = 9;                // "Het hoogste ooit gemeten UV-index in Nederland was 9,0 op 25 juni 1995"
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