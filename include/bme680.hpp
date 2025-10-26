#ifndef BME680_HPP
#define BME680_HPP

#include <Wire.h>
#include "Adafruit_BME680.h"

#define BME680_SDA 9
#define BME680_SCL 10

#define SEALEVELPRESSURE_HPA (1013.25)

extern TwoWire I2C_BME680;
extern Adafruit_BME680 bme;

void bme680Setup();
void debugI2CScan();
void i2cAutoDiagnose();
float getTemperature();
float getPressure();
float getHumidity();
float getGasResistance();
float getAltitude();
void printTemperature();
void printPressure();
void printHumidity();
void printGasResistance();
void printAltitude();

#endif // BME680_HPP