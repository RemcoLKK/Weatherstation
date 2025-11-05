#include <Arduino.h>
#include <oLed.hpp>
#include <wind.hpp>

// --- geometry from your design ---
static constexpr float CUP_DIAMETER_M   = 0.08f;   // 80 mm
static constexpr float CUP_OFFSET_M     = 0.155f;  // 155 mm
static constexpr uint8_t PULSES_PER_REV = 1;       // 1 magnet -> 1 pulse/rev
static constexpr float WIND_CAL = 1.0f;

// --- pulse counting ---
static volatile uint32_t pulseCount = 0;
static volatile uint32_t lastPulseMs = 0;

// ISR: count every time magnet passes
void IRAM_ATTR onWindPulse() {
  uint32_t now = millis();
  // simple debounce / noise filter: ignore pulses that are too close
  if (now - lastPulseMs > 2) {   // 3 ms = ~333 Hz max
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
  static uint32_t lastPulses = 0;

  uint32_t now = millis();
  uint32_t dt = now - lastCalc;
  if (dt >= 5000) {
      lastCalc = now;

      noInterrupts();
      uint32_t pulses = pulseCount;
      pulseCount = 0;
      interrupts();

      lastPulses = pulses;  // pulses counted in last 5 s

      float seconds = dt / 1000.0f;
      float rps = (float)pulses / (float)PULSES_PER_REV / seconds;
      float rpm = rps * 60.0f;
      Serial.print("Pulses in last 5s: ");
      Serial.println(lastPulses); 

      float ms = windSpeedFromRPM(rpm);
      ms *= WIND_CAL;
      lastSpeedKmh = ms * 3.6f;
  }

  return lastSpeedKmh;  // speed, not pulses
}


void printWindSpeedSerial() {
  float kmh = getWindSpeed();
  float ms  = kmh / 3.6f;

  Serial.print("Wind Speed: ");
  Serial.print(ms, 2);
  Serial.print(" m/s, ");
  Serial.print(kmh, 1);
  Serial.println(" km/h");
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
