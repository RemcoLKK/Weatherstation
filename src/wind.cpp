#include <Arduino.h>
#include <wind.hpp>
#include <oLed.hpp>

void windSetup(){
  analogSetPinAttenuation(WINDSPEED_PIN, ADC_0db);
  
}

float getWindSpeed() {
  int windSpeedValue = analogRead(WINDSPEED_PIN);
  int kmhWindSpeed = (windSpeedValue / 40);
  if (windSpeedValue > 30){
    kmhWindSpeed = kmhWindSpeed + 10;
  }
  return kmhWindSpeed;
}

void printWindSpeed() {
  display.clearDisplay(); 
  display.setCursor(0, 0);
  display.println(F("Wind Speed: "));   
  display.print(getWindSpeed());
  display.print(F(" KM/H"));
  display.display();
  delay(100);
}