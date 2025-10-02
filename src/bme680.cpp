#include "bme680.hpp"
#include <oLed.hpp>
 
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

void printTemperature() {
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Temp: "));
  display.print(bme.temperature - 1);
  display.println(F(" *C"));
  display.display();
  delay(100);
}

void printPressure() {
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Pressure: "));
  display.print(bme.pressure / 100.0);
  display.println(F(" hPa"));
  display.display();
  delay(100);
}

void printHumidity() {
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Humidity: "));
  display.print(bme.humidity);
  display.println(F(" %"));
  display.display();
  delay(100);
}

void printGasResistance() {
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Gas: "));
  display.print(bme.gas_resistance / 1000.0);
  display.println(F(" KOhms"));
  display.display();
  delay(100);
}

void printAltitude() {
  if (!bme.performReading()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Altitude: "));
  display.print(bme.readAltitude(SEALEVELPRESSURE_HPA) + 107); // -107 to calibrate to local altitude
  display.println(F(" m"));
  display.display();
  delay(100);
}