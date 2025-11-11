#ifndef SENDDATA_HPP
#define SENDDATA_HPP

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
#define WIFI_SSID           "Remco's netwerk 2,4G"
#define WIFI_PASSWORD       "Bananenplant10"
// #define WIFI_SSID           "Remco's S23"
// #define WIFI_PASSWORD       "Wachtwoord"

// MQTT settings
#define MQTT_BROKER         "145.24.237.211"

#define MQTT_TEMPERATURE    "homestations/1029441/0/temperature"
#define MQTT_HUMIDITY       "homestations/1029441/0/humidity"
#define MQTT_PRESSURE       "homestations/1029441/0/airpressure"   
#define MQTT_GAS            "homestations/1029441/0/gasresistance"
#define MQTT_ALTITUDE       "homestations/1029441/0/altitude"
#define MQTT_WINDSPEED      "homestations/1029441/0/windspeed"
#define MQTT_UV             "homestations/1029441/0/uv"
#define MQTT_GPS            "homestations/1029441/0/gps"
#define MQTT_LIGHT          "homestations/1029441/0/light"
#define MQTT_UPDATE         "homestations/1029441/0/update"

#define MQTT_USERNAME       "minor_smart_things"
#define MQTT_PASSWORD       "smart_things_2025"
#define MQTT_PORT           8883

extern WiFiClient espClient;
extern PubSubClient mqtt;
bool ensureConnected();
void connectWiFi();
void connectMQTT();
void startMqttTaskOnCore0();
void mqttPublish();

#endif // SENDDATA_HPP
