#include <dht11.hpp>
#include <oLed.hpp>

DHT dht(DHTPIN, DHTTYPE);

void dht11Setup() {
  dht.begin();
}

float getTemperatureDHT11() {
    float temp = dht.readTemperature() - 1.0; 
    return temp;
}

void printTemperatureDHT11() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;

  float temp = getTemperatureDHT11();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Temp: "));
  display.print(temp);
  display.println(F(" °C"));
  display.display();
}