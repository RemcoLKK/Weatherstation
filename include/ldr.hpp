#ifndef LDR_HPP
#define LDR_HPP

#define LDRN_PIN 4
#define LDRE_PIN 5
#define LDRS_PIN 15
#define LDRW_PIN 7

void LDRSetup();
void updateLDRs();
int getMostSun();
void printMostSun();
void printLDRValues();
#endif // UV_HPP