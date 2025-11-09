#include "bme680.hpp"
#include <oLed.hpp>
#include <dht11.hpp>

Adafruit_BME680 bme;

void bme680Setup() {

  if (!bme.begin(0x76, &Wire)) {
    Serial.println(F("❌ BME680 not found"));
    while (1);
  }

  Serial.println(F("✅ BME680 sensor initialized"));

  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);
}

void debugI2CScan() {
  byte error, address;
  int devices = 0;

  Serial.println(F("Scanning I2C bus..."));

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("I2C device found at 0x"));
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
      devices++;
    } 
    else if (error == 4) {
      Serial.print(F("Unknown error at 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (devices == 0)
    Serial.println(F("No I2C devices found"));
  else
    Serial.printf("Scan complete, %d device(s) found\n", devices);
  
  Serial.println(F("----------------------------"));
  delay(500);
}

float getTemperature() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    return -999;
  }
  return bme.temperature - 4;
}

float getPressure() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    return -999;
  }
  return bme.pressure / 100.0;
}

float getHumidity() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    return -999;
  }
  return bme.humidity;
}

float getGasResistance() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    return -999;
  }
  return bme.gas_resistance / 1000.0;
}

float getAltitude() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    return -999;
  }
  return bme.readAltitude(SEALEVELPRESSURE_HPA) + 137; // -107 to calibrate to local altitude
}

void printTemperature() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;

  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Temp: "));
  display.print(getTemperature());
  display.println(F(" *C"));
  display.display();
}

void printTemperatureCombined() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;

  float bmeTemp = getTemperature();
  float dhtTemp = getTemperatureDHT11();
  float avgTemp = (bmeTemp + dhtTemp) / 2.0f;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Temp: "));
  display.print(avgTemp);
  display.println(F(" *C"));
  display.display();
}

void printPressure() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;

  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Pressure: "));
  display.println(bme.pressure / 100.0);
  display.println(F("hPa"));
  display.display();
}

void printHumidity() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;

  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Humidity: "));
  display.print(bme.humidity);
  display.println(F(" %"));
  display.display();
}

void printGasResistance() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;
  
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Gas: "));
  display.println(bme.gas_resistance / 1000.0);
  display.println(F("KOhms"));
  display.display();
}

void printAltitude() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 100) return;
  lastUpdate = now;
  
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Altitude: "));
  display.print(getAltitude()); // -107 to calibrate to local altitude
  display.println(F(" m"));
  display.display();
}