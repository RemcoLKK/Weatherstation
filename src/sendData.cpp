//make core 0 send all data to the server
#include <sendData.hpp>
#include <uv.hpp>
#include <wind.hpp>
#include <ldr.hpp>
#include <bme680.hpp>
#include <rgb.hpp>

WiFiClient espClient;
PubSubClient mqtt(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;;
const unsigned long PUBLISH_PERIOD_MS = 5000;
unsigned long nextPublish = 0;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  setNetLed(WIFI_CONNECTING);            // <— RED flicker while we try
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    netLedUpdate();                      // keep flickering during the wait
    Serial.print(".");
    delay(120);                          // short so flicker feels responsive
  }

  Serial.print("\nIP: "); Serial.println(WiFi.localIP());
  IPAddress dns1(8,8,8,8), dns2(8,8,4,4);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, dns1, dns2);
  Serial.print("DNS0: "); Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS1: "); Serial.println(WiFi.dnsIP(1));
}

// 3) MQTT connect (simple)
void connectMQTT() {
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  if (mqtt.connected()) return;

  setNetLed(MQTT_CONNECTING);            // <— BLUE flicker during connect try

  String clientId = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  bool ok = (MQTT_USERNAME[0] == '\0')
              ? mqtt.connect(clientId.c_str())
              : mqtt.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD);

  if (ok) {
    Serial.println("MQTT: connected");
    setNetLed(ONLINE);                   // both should be up now
  } else {
    Serial.printf("MQTT: connect failed, rc=%d\n", mqtt.state());
    // leave it in MQTT_CONNECTING; ensureConnected() will retry
  }
}

// --- Ensure connections ---
bool ensureConnected() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (WiFi.status() == WL_CONNECTED && !mqtt.connected()) connectMQTT();
  return WiFi.status() == WL_CONNECTED && mqtt.connected();
}

// --- Publish all sensors ---
void publishAll() {
  if (!mqtt.connected()) return;

  // helper lambda to format and publish
  auto send = [&](const char* topic, float value) {
    char payload[16];
    dtostrf(value, 0, 2, payload);  // float → string conversion
    mqtt.publish(topic, payload);
    Serial.printf("→ %s: %s\n", topic, payload);
    delay(10); // short delay for broker
  };

  auto sendInt = [&](const char* topic, int value) {
    char payload[16];
    snprintf(payload, sizeof(payload), "%d", value);  // int → string conversion
    mqtt.publish(topic, payload);
    Serial.printf("→ %s: %s\n", topic, payload);
    delay(10); // short delay for broker
  };

  send(MQTT_TEMPERATURE,  getTemperature());
  send(MQTT_HUMIDITY,     getHumidity());
  send(MQTT_PRESSURE,     getPressure());
  send(MQTT_GAS,          getGasResistance());
  send(MQTT_ALTITUDE,     getAltitude());
  send(MQTT_WINDSPEED,    getWindSpeed());
  sendInt(MQTT_UV,        getUVLevel());

  Serial.println("MQTT: all sensor values published\n");
}

void mqttPublish() {
  // keep status in sync
  if (WiFi.status() != WL_CONNECTED)      setNetLed(WIFI_CONNECTING);
  else if (!mqtt.connected())             setNetLed(MQTT_CONNECTING);
  else                                    setNetLed(ONLINE);

  netLedUpdate();                         // tick the LED state machine

  mqtt.loop();
  unsigned long now = millis();
  if ((long)(now - nextPublish) >= 0) {
    if (ensureConnected()) publishAll();
    nextPublish = now + PUBLISH_PERIOD_MS;
  }
}
