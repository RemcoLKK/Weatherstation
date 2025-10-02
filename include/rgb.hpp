#ifndef RGB_HPP
#define RGB_HPP

#include <Adafruit_NeoPixel.h>

#define PIN         48
#define NUMPIXELS   1
#define FADE_DELAY  10   // ms between color-steps; lower = faster
#define LED_BRIGHTNESS 15  // 0 = off, 255 = full

void rgbSetup();
void rgbWheel();
void rgbTask(void* pvParameters);

#endif