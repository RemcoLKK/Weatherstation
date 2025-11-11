#ifndef LDR_HPP
#define LDR_HPP

#define LDRN_PIN 15
#define LDRE_PIN 4
#define LDRS_PIN 5
#define LDRW_PIN 7

void LDRSetup();
void updateLDRs();
int getMostSun();
int getMostSunPin();
float getMostSunLux();
void printMostSun();
void printLux();
void printLDRValues();
#endif // UV_HPP