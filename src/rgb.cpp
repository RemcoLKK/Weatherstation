#include <rgb.hpp>

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// --- Make these static so they persist between calls ---
static uint16_t wheelPos = 0;
static unsigned long lastUpdate = 0;

uint32_t wheel(byte pos) {
  if (pos < 85) {
    return pixels.Color(pos * 3, 255 - pos * 3, 0);
  } else if (pos < 170) {
    pos -= 85;
    return pixels.Color(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return pixels.Color(0, pos * 3, 255 - pos * 3);
  }
}

void rgbSetup() {
  pixels.begin();
  pixels.setBrightness(LED_BRIGHTNESS); // define this in a header or macro
  pixels.clear();
  pixels.show();
}

void rgbWheel() {
  unsigned long now = millis();
  if (now - lastUpdate >= FADE_DELAY) {
    lastUpdate = now;

    pixels.setPixelColor(0, wheel(wheelPos));
    pixels.show();

    wheelPos++;
    if (wheelPos >= 256) wheelPos = 0;
  }
}

void rgbTask(void* pvParameters) {
  while (true) {
    rgbWheel();
    delay(20);  // of millis()-gebaseerd als je die al hebt
  }
}
