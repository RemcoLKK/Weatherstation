#include <ldr.hpp>
#include <Arduino.h>
#include <oLed.hpp>

int getMostSun() {
    int values[4] = {
        analogRead(LDRN_PIN), // NORTH
        analogRead(LDRE_PIN), // EAST
        analogRead(LDRS_PIN), // SOUTH
        analogRead(LDRW_PIN)  // WEST
    };

    int maxIndex = 0;
    for (int i = 1; i < 4; i++) {
        if (values[i] > values[maxIndex]) {
            maxIndex = i;
        }
    }

    return maxIndex; // 0 = NORTH, 1 = EAST, 2 = SOUTH, 3 = WEST
}

void printMostSun() {
  display.clearDisplay(); 
  display.setCursor(0, 0);
  display.print(F("Most sun is: "));   
  display.println(getMostSun());
  display.display();
  delay(100);
}