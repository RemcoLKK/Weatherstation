#include <Arduino.h>
#include <oLed.hpp>
#include <uv.hpp>
#include <wind.hpp>
#include <ldr.hpp>
#include <rgb.hpp>
#include <bme680.hpp>

void setup() {
  Serial.begin(115200);
  pinMode(WINDSPEED_PIN, INPUT);
  pinMode(UV_PIN, INPUT);
  rgbSetup();
  xTaskCreatePinnedToCore(
    rgbTask,        // functie
    "RGB Task",     // naam
    2048,           // stack size (kan kleiner/groter)
    NULL,           // parameters
    1,              // prioriteit
    NULL,           // task handle
    0               // 👈 Core 0
  );
  windSetup();
  oLedSetup();
  bme680Setup();
}

void loop() {
  printUVLevel();
  delay(1000); //maak nog een switch case voor de knoppen ipv delays in loop
  printWindSpeed();
  delay(1000);
  printMostSun();
  delay(1000);
  printTemperature();
  delay(1000);
  printPressure();
  delay(1000);
  printHumidity();
  delay(1000);
  printGasResistance();
  delay(1000);
  printAltitude();
  delay(1000);
}