#include <ldr.hpp>
#include <Arduino.h>
#include <oLed.hpp>

// --- config ---
static const int LDR_PINS[4] = { LDRN_PIN, LDRE_PIN, LDRS_PIN, LDRW_PIN };
static float ldrAvg[4] = {0,0,0,0};
static bool ldrInitDone = false;
static const float ALPHA = 0.15f;        // smoothing (0..1), lower = smoother
static const uint32_t SAMPLE_MS = 20;    // ~50 Hz sample/update

// --- optional hysteresis to avoid flapping between close values ---
static int lastWinner = -1;
static const float HYST_ABS = 60.0f;     // ~60 ADC counts absolute
static const float HYST_PCT = 0.05f;     // or 5% of current max

void LDRSetup() {
  for (int i = 0; i < 4; ++i) {
    pinMode(LDR_PINS[i], INPUT);
    analogSetPinAttenuation(LDR_PINS[i], ADC_11db);
  }
  analogReadResolution(12);

  // seed EMA with first measurements (avoid “cold start” zero bias)
  for (int i = 0; i < 4; ++i) {
    (void)analogRead(LDR_PINS[i]);                 // throw away first read
    ldrAvg[i] = (float)analogRead(LDR_PINS[i]);    // seed
  }
  ldrInitDone = true;
}

// call this frequently in loop(); it self-throttles
void updateLDRs() {
  static uint32_t last = 0;
  uint32_t now = millis();
  if (now - last < SAMPLE_MS) return;
  last = now;

  for (int i = 0; i < 4; ++i) {
    (void)analogRead(LDR_PINS[i]);                 // settle mux
    int r = analogRead(LDR_PINS[i]);               // actual sample
    ldrAvg[i] += ALPHA * (r - ldrAvg[i]);          // EMA
  }
}

// pick the brightest with hysteresis (prevents rapid flipping)
int getMostSun() {
  if (!ldrInitDone) return 0;

  // find current max index from averages
  int maxIdx = 0;
  float maxVal = ldrAvg[0];
  for (int i = 1; i < 4; ++i) {
    if (ldrAvg[i] > maxVal) { maxVal = ldrAvg[i]; maxIdx = i; }
  }

  if (lastWinner < 0) { lastWinner = maxIdx; return lastWinner; }

  float delta   = ldrAvg[maxIdx] - ldrAvg[lastWinner];
  float thresh  = max(HYST_ABS, HYST_PCT * maxVal);

  if (delta > thresh) lastWinner = maxIdx; // switch only if meaningfully brighter
  return lastWinner;                        // 0=N, 1=E, 2=S, 3=W
}

// --- debugging/visualization ---
void printLDRValues(){
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("N: "); display.println((int)(ldrAvg[0] + 0.5f));
  display.print("E: "); display.println((int)(ldrAvg[1] + 0.5f));
  display.print("S: "); display.println((int)(ldrAvg[2] + 0.5f));
  display.print("W: "); display.println((int)(ldrAvg[3] + 0.5f));
  display.display();
  // (avoid delay if you want max responsiveness)
}

void printMostSun() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Most sun: "));
  display.println(getMostSun());
  display.display();
}
