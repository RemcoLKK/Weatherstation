#include "rgb.hpp"

static Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// --- simple state for non-blocking flicker ---
static NetLedMode s_mode = LED_OFF;
static unsigned long s_t = 0;
static bool s_on = false;

static inline void showPixel(uint8_t r, uint8_t g, uint8_t b) {
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show();
}

void rgbSetup() {
  pixels.begin();
  pixels.setBrightness(LED_BRIGHTNESS);
  pixels.clear();
  pixels.show();
  s_mode = LED_OFF;
  s_on = false;
  s_t = 0;
}

void setNetLed(NetLedMode m) {
  if (s_mode != m) {
    s_mode = m;
    s_on = false;
    s_t = 0;
  }
}

void netLedUpdate() {
  const unsigned long period = 160; // ms (80 on / 80 off)
  unsigned long now = millis();

  switch (s_mode) {
    case WIFI_CONNECTING: // red flicker
      if (now - s_t >= period) { s_t = now; s_on = !s_on; }
      showPixel(s_on ? 255 : 0, 0, 0);
      break;

    case MQTT_CONNECTING: // blue flicker
      if (now - s_t >= period) { s_t = now; s_on = !s_on; }
      showPixel(0, 0, s_on ? 255 : 0);
      break;

    case ONLINE:          // solid green
      showPixel(0, 255, 0);
      break;

    default:              // LED_OFF
      showPixel(0, 0, 0);
      break;
  }
}
