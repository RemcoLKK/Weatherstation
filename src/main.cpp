#include <Arduino.h>
#include <oLed.hpp>
#include <uv.hpp>
#include <wind.hpp>
#include <ldr.hpp>
#include <sendData.hpp>
#include <bme680.hpp>
#include <dht11.hpp>
#include <servo.hpp>
#include <rgb.hpp>
#include <buttons.hpp>

void setup() {
  Serial.begin(115200);
  Serial.setTxBufferSize(4096);
  pinMode(UV_PIN, INPUT_PULLDOWN);
  LDRSetup();
  windSetup();
  servoSetup();
  oLedSetup();
  bme680Setup();
  rgbSetup();   
  buttonSetup();             
  setNetLed(LED_OFF);
  connectWiFi();
  connectMQTT();
  startMqttTaskOnCore0();
}

void loop() {
  updateLDRs(); 
  rotateServo();
  ensureConnected();
  // mqttPublish();
  handleButtons();

switch (currentScreen) {
    case 0: printUVLevel();               break;
    case 1: printWindSpeed();             break;
    case 2: printMostSun();               break; 
    case 3: printTemperatureCombined();   break;
    case 4: printPressure();              break;
    case 5: printHumidity();              break;
    case 6: printGasResistance();         break;
    case 7: printAltitude();              break;
    case 8: printLux();                   break;
  }
}