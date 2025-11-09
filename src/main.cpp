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

// --- button flags set from interrupts ---
volatile bool nextPressedFlag = false;
volatile bool prevPressedFlag = false;

void IRAM_ATTR onNextPress() {
  nextPressedFlag = true;   // just set a flag, keep ISR tiny
}

void IRAM_ATTR onPrevPress() {
  prevPressedFlag = true;
}

void handleButtons() {
  static uint32_t lastNextHandled = 0;
  static uint32_t lastPrevHandled = 0;
  uint32_t now = millis();

  // --- NEXT button ---
  if (nextPressedFlag) {
    noInterrupts();
    bool pressed = nextPressedFlag;
    nextPressedFlag = false;   // clear the flag
    interrupts();

    if (pressed && (now - lastNextHandled) > 120) {  // 120 ms debounce
      lastNextHandled = now;
      currentScreen++;
      if (currentScreen >= totalScreens) currentScreen = 0;
    }
  }

  // --- PREV button ---
  if (prevPressedFlag) {
    noInterrupts();
    bool pressed = prevPressedFlag;
    prevPressedFlag = false;
    interrupts();

    if (pressed && (now - lastPrevHandled) > 120) {
      lastPrevHandled = now;
      currentScreen--;
      if (currentScreen < 0) currentScreen = totalScreens - 1;
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.setTxBufferSize(4096);
  pinMode(UV_PIN, INPUT_PULLDOWN);
  pinMode(BTN_NEXT, INPUT);
  pinMode(BTN_PREV, INPUT);
  attachInterrupt(digitalPinToInterrupt(BTN_NEXT), onNextPress, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_PREV), onPrevPress, RISING);
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
    case 0: printUVLevel();               break;
    case 1: printWindSpeed();             break;
    case 2: printMostSun();               break; 
    case 3: printTemperatureCombined();   break;
    case 4: printPressure();              break;
    case 5: printHumidity();              break;
    case 6: printGasResistance();         break;
    case 7: printAltitude();              break;
  }
}