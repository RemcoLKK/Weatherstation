#include <Arduino.h>
#include <oLed.hpp>
#include <uv.hpp>
#include <wind.hpp>
#include <ldr.hpp>
#include <sendData.hpp>
#include <bme680.hpp>
#include <servo.hpp>
#include <rgb.hpp>

#define BTN_NEXT  16   // button to go forward
#define BTN_PREV  17   // button to go backward

int currentScreen = 0;
const int totalScreens = 8;
unsigned long lastDebounce = 0;
const unsigned long debounceDelay = 150; // ms

void setup() {
  Serial.begin(115200);
  
  pinMode(WINDSPEED_PIN, INPUT);
  pinMode(UV_PIN, INPUT);
  pinMode(BTN_NEXT, INPUT);
  pinMode(BTN_PREV, INPUT);

  windSetup(); // rewrite to hall sensor
  // servoSetup();
  rgbSetup();
  oLedSetup();
  bme680Setup();
  rgbSetup();                 // LED ready
  setNetLed(LED_OFF);
  connectWiFi();
  connectMQTT();
}

void loop() {
  // rotateServo();
  ensureConnected();
  mqttPublish();
  // if (millis() - lastDebounce > debounceDelay) {
  //   if (digitalRead(BTN_NEXT) == HIGH) {
  //     currentScreen++;
  //     if (currentScreen >= totalScreens) currentScreen = 0;
  //     lastDebounce = millis();
  //   }
  //   if (digitalRead(BTN_PREV) == HIGH) {
  //     currentScreen--;
  //     if (currentScreen < 0) currentScreen = totalScreens - 1;
  //     lastDebounce = millis();
  //   }
  // }

switch (currentScreen) {
    case 0:
      printUVLevel();
      break;
    case 1:
      printWindSpeed();
      break;
    case 2:
      printMostSun();
      break; 
    case 3:
      printTemperature();
      break;
    case 4:
      printPressure();
      break;
    case 5:
      printHumidity();
      break;
    case 6:
      printGasResistance();
      break;
    case 7:
      printAltitude();
      break;
  }
}