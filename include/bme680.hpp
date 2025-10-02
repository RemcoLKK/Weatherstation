#ifndef BME680_HPP
#define BME680_HPP

#include <Wire.h>
#include "Adafruit_BME680.h"

#define BME680_SDA 12
#define BME680_SCL 11

#define SEALEVELPRESSURE_HPA (1013.25)

extern TwoWire I2C_BME680;
extern Adafruit_BME680 bme;

void bme680Setup();
void printTemperature();
void printPressure();
void printHumidity();
void printGasResistance();
void printAltitude();

#endif // BME680_HPP