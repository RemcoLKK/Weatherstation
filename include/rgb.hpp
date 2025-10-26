#ifndef RGB_HPP
#define RGB_HPP

#include <Adafruit_NeoPixel.h>

#define PIN             38
#define NUMPIXELS       1
#define LED_BRIGHTNESS  15  // 0..255

// --- Net status LED API ---
enum NetLedMode : uint8_t { LED_OFF, WIFI_CONNECTING, MQTT_CONNECTING, ONLINE };

// Call once in setup
void rgbSetup();

// Call often (loop / timer) to animate
void netLedUpdate();

// Set the current mode (red flicker, blue flicker, solid green, off)
void setNetLed(NetLedMode m);

#endif
