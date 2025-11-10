#include <Arduino.h>
#include <buttons.hpp>

int currentScreen = 0;
const int totalScreens = 9;

// --- button flags set from interrupts ---
volatile bool nextPressedFlag = false;
volatile bool prevPressedFlag = false;

void IRAM_ATTR onNextPress() {
  nextPressedFlag = true;   // just set a flag, keep ISR tiny
}

void IRAM_ATTR onPrevPress() {
  prevPressedFlag = true;
}

void buttonSetup(){
    pinMode(BTN_NEXT, INPUT_PULLDOWN);
    pinMode(BTN_PREV, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(BTN_NEXT), onNextPress, RISING);
    attachInterrupt(digitalPinToInterrupt(BTN_PREV), onPrevPress, RISING);
}

void handleButtons() {
  static uint32_t lastNextHandled = 0;
  static uint32_t lastPrevHandled = 0;
  static uint32_t buttonDebounceMs = 300;
  uint32_t now = millis();

  // --- NEXT button ---
  if (nextPressedFlag) {
    noInterrupts();
    bool pressed = nextPressedFlag;
    nextPressedFlag = false;   // clear the flag
    interrupts();

    if (pressed && (now - lastNextHandled) > buttonDebounceMs) {  // 120 ms debounce
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

    if (pressed && (now - lastPrevHandled) > buttonDebounceMs) {
      lastPrevHandled = now;
      currentScreen--;
      if (currentScreen < 0) currentScreen = totalScreens - 1;
    }
  }
}