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

static void i2cBusStatus(int SDA, int SCL) {
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delay(2);
  int sda = digitalRead(SDA);
  int scl = digitalRead(SCL);
  Serial.printf("   Lines: SDA=%s SCL=%s (with INPUT_PULLUP)\n",
                sda ? "HIGH" : "LOW", scl ? "HIGH" : "LOW");
  if (!sda || !scl) {
    Serial.println("   ⚠️ One or both lines LOW. Check pull-ups/shorts/device holding line.");
  }
}

// ---- Try to recover a stuck bus by clocking SCL and issuing a STOP ----
// Call ONLY when SDA reads LOW (device may be mid-byte).
static void i2cRecover(int SDA, int SCL) {
  Serial.println("   Attempting I2C bus recovery…");
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delay(2);

  if (digitalRead(SDA) == LOW) {
    // Use open-drain style toggling on SCL, gently clock up to 9 pulses
    pinMode(SCL, OUTPUT_OPEN_DRAIN);
    digitalWrite(SCL, HIGH); // release
    for (int i = 0; i < 9 && digitalRead(SDA) == LOW; i++) {
      digitalWrite(SCL, LOW);  delayMicroseconds(5);
      digitalWrite(SCL, HIGH); delayMicroseconds(5);
    }
    // Issue a STOP: SDA low -> SCL high -> release SDA high
    pinMode(SDA, OUTPUT_OPEN_DRAIN);
    digitalWrite(SDA, LOW);    delayMicroseconds(5);
    digitalWrite(SCL, HIGH);   delayMicroseconds(5);
    digitalWrite(SDA, HIGH);   delayMicroseconds(5);
  }

  // Return to input-pullups for a sanity check
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delay(2);
  Serial.printf("   After recovery: SDA=%s SCL=%s\n",
                digitalRead(SDA) ? "HIGH" : "LOW",
                digitalRead(SCL) ? "HIGH" : "LOW");
}

// ---- Classic scanner on a given pin pair and frequency ----
static int i2cScanOnce(int SDA, int SCL, uint32_t hz) {
  int found = 0;
  Wire.end(); // ensure clean state
  Wire.begin(SDA, SCL, hz);
  delay(20);

  Serial.printf("🔍 Scanning on SDA=%d SCL=%d @ %lu Hz\n", SDA, SCL, (unsigned long)hz);
  i2cBusStatus(SDA, SCL);

  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.printf("   ✅ Device @ 0x%02X\n", addr);
      found++;
    } else if (err == 4) {
      Serial.printf("   ⚠️ Unknown error @ 0x%02X\n", addr);
    }
  }
  if (found == 0) Serial.println("   ❌ No devices on this pin/speed combo.");
  Serial.println("--------------------------------------------------");
  return found;
}

// ---- Try multiple common pin pairs and speeds on ESP32/S3 ----
// Adjust the pin pairs to match your wiring if needed.
void i2cAutoDiagnose() {
  Serial.println("\n=== I2C Auto-Diagnose ===");

  // Common SDA/SCL pairs people use on ESP32/ESP32-S3 devkits.
  // Edit this list to include your wiring first to save time.
  const int PIN_PAIRS[][2] = {
    {21, 22},   // classic ESP32 default
    {9, 8},     // popular on many S3 devkits (SDA=9, SCL=8)
    {7, 6},     // also seen on S3 builds
    {11, 12},   // alternative
    {5, 4},     // spare GPIOs often free
  };
  const uint32_t SPEEDS[] = {100000, 400000}; // 100 kHz, 400 kHz

  for (auto &pair : PIN_PAIRS) {
    int SDA = pair[0], SCL = pair[1];

    // Check line levels; if SDA is stuck LOW, try recovery
    pinMode(SDA, INPUT_PULLUP);
    pinMode(SCL, INPUT_PULLUP);
    delay(2);
    if (digitalRead(SDA) == LOW) {
      i2cRecover(SDA, SCL);
    }

    for (uint32_t hz : SPEEDS) {
      int devices = i2cScanOnce(SDA, SCL, hz);
      if (devices > 0) {
        Serial.printf("🎯 Success on SDA=%d SCL=%d @ %lu Hz\n", SDA, SCL, (unsigned long)hz);
        Serial.println("Tip: Use these pins/speed in Wire.begin() for your project.\n");
        return; // stop at first success
      }
    }
  }

  Serial.println("🚫 Still nothing found. See checklist below.\n");
}

float getTemperature() {
  if (!bme.performReading()) {
    Serial.println(F("Failed to perform reading :("));
    return -999;
  }
  return bme.temperature - 1;
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
  return bme.readAltitude(SEALEVELPRESSURE_HPA) - 134; // -107 to calibrate to local altitude
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
  display.print(bme.readAltitude(SEALEVELPRESSURE_HPA) - 107); // -107 to calibrate to local altitude
  display.println(F(" m"));
  display.display();
  delay(100);
}