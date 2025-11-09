#include <dht11.hpp>
#include <oLed.hpp>

DHT dht(DHTPIN, DHTTYPE);

void dht11Setup() {
    dht.begin();
}

float getTemperatureDHT11() {
  static float lastValid = NAN;        
  float t = dht.readTemperature() - 3.0;   // your calibration

  // 1) basic sanity check on the raw value
  if (isnan(t) || t < -10 || t > 60) {
    Serial.print(F("DHT invalid raw: "));
    Serial.println(t);
    return lastValid;
  }

  // 2) first valid reading: be picky
  if (isnan(lastValid)) {
    // assume indoor, so ignore really weird first values
    if (t < 10 || t > 35) {      // tweak this range if needed
      Serial.print(F("DHT first reading suspicious: "));
      Serial.println(t);
      return lastValid;          // still NAN, caller should handle that
    }
    lastValid = t;
    return lastValid;
  }

  // 3) spike filter: allow big rises, block big drops
  //    (your problem case was 22 -> 11, not 9 -> 22)
  if (t < lastValid - 5.0f) {    // drop > 5°C in one step? very unlikely
    Serial.print(F("DHT spike down filtered: "));
    Serial.print(t);
    Serial.print(F(" (last="));
    Serial.print(lastValid);
    Serial.println(F(")"));
    return lastValid;
  }

  // optional: smooth changes a bit instead of jumping
  // lastValid = lastValid + 0.3f * (t - lastValid);  // 0.0–1.0 (0.3 = light smoothing)
  lastValid = t;   // or keep it simple, no smoothing

  return lastValid;
}

void printTemperatureDHT11() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 1000) return;
  lastUpdate = now;

  float temp = getTemperatureDHT11();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Temp: "));
  display.print(temp);
  display.println(F(" *C"));
  display.display();
}