#include <Arduino.h>
#include <oLed.hpp>
#include <uv.hpp>
#include <wind.hpp>
#include <ldr.hpp>
#include <sendData.hpp>
#include <bme680.hpp>
#include <dht11.hpp>
#include <servo.hpp>
#include <rgb.hpp>

#define BTN_NEXT  16   // button to go forward
#define BTN_PREV  17   // button to go backward

int currentScreen = 0;
const int totalScreens = 8;
const unsigned long debounceDelay = 80; // ms
bool lastNextState = LOW;  
bool lastPrevState = LOW;
unsigned long lastNextDebounce = 0;
unsigned long lastPrevDebounce = 0;
bool nextHandled = false;
bool prevHandled = false;

void handleButtons() {
  unsigned long now = millis();

  // --- NEXT button ---
  bool nextReading = digitalRead(BTN_NEXT);
  if (nextReading != lastNextState) {
    lastNextDebounce = now;
    lastNextState = nextReading;
  }
  if ((now - lastNextDebounce) > debounceDelay && nextReading == HIGH) {
    if (!nextHandled) {
      currentScreen++;
      if (currentScreen >= totalScreens) currentScreen = 0;
      nextHandled = true;
    }
  } else if (nextReading == LOW) {
    nextHandled = false;  // reset when released
  }

  // --- PREV button ---
  bool prevReading = digitalRead(BTN_PREV);
  if (prevReading != lastPrevState) {
    lastPrevDebounce = now;
    lastPrevState = prevReading;
  }
  if ((now - lastPrevDebounce) > debounceDelay && prevReading == HIGH) {
    if (!prevHandled) {
      currentScreen--;
      if (currentScreen < 0) currentScreen = totalScreens - 1;
      prevHandled = true;
    }
  } else if (prevReading == LOW) {
    prevHandled = false;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTxBufferSize(4096);
  pinMode(UV_PIN, INPUT_PULLDOWN);
  pinMode(BTN_NEXT, INPUT);
  pinMode(BTN_PREV, INPUT);
  LDRSetup();
  windSetup(); // rewrite to hall sensor
  servoSetup();
  oLedSetup();
  bme680Setup();
  rgbSetup();                 // LED ready
  setNetLed(LED_OFF);
  connectWiFi();
  connectMQTT();
}

void loop() {
  updateLDRs(); 
  rotateServo();
  ensureConnected();
  mqttPublish();
  handleButtons();

switch (currentScreen) {
    case 0: printUVLevel();           break;
    case 1: printWindSpeed();         break;
    case 2: printMostSun();           break; 
    case 3: printTemperatureDHT11();  break;
    case 4: printPressure();          break;
    case 5: printHumidity();          break;
    case 6: printGasResistance();     break;
    case 7: printAltitude();          break;
  }
}