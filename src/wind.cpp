#include <Arduino.h>
#include <oLed.hpp>
#include <wind.hpp>

// --- geometry from your design ---
static constexpr float CUP_DIAMETER_M   = 0.08f;   // 80 mm
static constexpr float CUP_OFFSET_M     = 0.155f;  // 155 mm
static constexpr uint8_t PULSES_PER_REV = 1;       // 1 magnet -> 1 pulse/rev

// --- pulse counting ---
static volatile uint32_t pulseCount = 0;
static volatile uint32_t lastPulseMs = 0;

// ISR: count every time magnet passes
void IRAM_ATTR onWindPulse() {
  uint32_t now = millis();
  // simple debounce / noise filter: ignore pulses that are too close
  if (now - lastPulseMs > 3) {   // 3 ms = ~333 Hz max
    pulseCount++;
    lastPulseMs = now;
  }
}

void windSetup() {
  pinMode(WINDSPEED_PIN, INPUT_PULLUP);   // A3144 needs pull-up
  attachInterrupt(digitalPinToInterrupt(WINDSPEED_PIN), onWindPulse, FALLING);
}

// calculate m/s from RPM using your formula:
// v = (π * D * RPM) / (2 * C)
static float windSpeedFromRPM(float rpm) {
  // v in m/s
  return (PI * CUP_DIAMETER_M * rpm) / (60 * 2.0f * CUP_OFFSET_M);
}

// call this every second (or whatever interval) to get current speed
float getWindSpeed() {
  static uint32_t lastCalc = 0;
  static float lastSpeedKmh = 0.0f;

  uint32_t now = millis();
  if (now - lastCalc >= 2000) {          // 1-second window
    lastCalc = now;

    noInterrupts();
    uint32_t pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    // pulses per second -> RPM
    // pulses_per_second = pulses / 1s
    // revolutions_per_second = pulses / PULSES_PER_REV
    // RPM = RPS * 60
    float rps = (float)pulses / (float)PULSES_PER_REV;
    float rpm = rps * 60.0f;

    float ms = windSpeedFromRPM(rpm);  // m/s
    lastSpeedKmh = ms * 3.6f;
  }

  return lastSpeedKmh;  // m/s (last measured)
}

void printWindSpeed() {
  float kmh = getWindSpeed();
  float ms  = kmh / 3.6f;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Wind Speed:"));
  display.print(ms, 2);
  display.println(F(" m/s"));
  display.print(kmh, 1);
  display.println(F(" km/h"));
  display.display();
}
