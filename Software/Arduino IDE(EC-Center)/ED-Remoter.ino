#include "WiFi.h"
#include "PubSubClient.h"

#define LEDG_PIN      0
#define LEDB_PIN      1
#define REMOTERK1_PIN 7
#define REMOTERK2_PIN 6
#define REMOTERK3_PIN 5
#define REMOTERK4_PIN 4

#define LEDG_ON()           digitalWrite(LEDG_PIN, LOW)
#define LEDG_OFF()          digitalWrite(LEDG_PIN, HIGH)
#define LEDB_ON()           digitalWrite(LEDB_PIN, LOW)
#define LEDB_OFF()          digitalWrite(LEDB_PIN, HIGH)
#define REMOTERK1_PRESS()   digitalWrite(REMOTERK1_PIN, HIGH)
#define REMOTERK1_RELEASE() digitalWrite(REMOTERK1_PIN, LOW)
#define REMOTERK2_PRESS()   digitalWrite(REMOTERK2_PIN, HIGH)
#define REMOTERK2_RELEASE() digitalWrite(REMOTERK2_PIN, LOW)
#define REMOTERK3_PRESS()   digitalWrite(REMOTERK3_PIN, HIGH)
#define REMOTERK3_RELEASE() digitalWrite(REMOTERK3_PIN, LOW)
#define REMOTERK4_PRESS()   digitalWrite(REMOTERK4_PIN, HIGH)
#define REMOTERK4_RELEASE() digitalWrite(REMOTERK4_PIN, LOW)

const char *wifi_ssid = "WiFi";
const char *wifi_password = "q7njkdjf";

const char *mqtt_broker = "xuyingjing.top";
const char *topic_sub = "ED-Remoter";
const char *topic_cb = "ED-RemoterCb";
const char *mqtt_username = "ED-Remoter";
const char *mqtt_password = "977930";
const short mqtt_port = 1883;
String client_id = "ED-Remoter_";

WiFiClient espClient;
PubSubClient client(espClient);

boolean is_payload_same_cmd(uint8_t *payload, const char *cmd, unsigned int length) {
  unsigned int index;

  for (index = 0; index < length; index++) {
    if (payload[index] != cmd[index]) {
      return false;
    }
  }

  return true;
}

void mqtt_callback(char *topic, uint8_t *payload, unsigned int length) {
  if (strcmp(topic, topic_sub) == 0) {
    if (is_payload_same_cmd(payload, "Up", length) == true) {
      REMOTERK1_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK1_RELEASE();
      client.publish(topic_cb, "Up");
    }
    else if (is_payload_same_cmd(payload, "UpLong", length) == true) {
      REMOTERK4_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK4_RELEASE();
      delay(250);
      REMOTERK1_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK1_RELEASE();
      client.publish(topic_cb, "UpLong");
    }
    else if (is_payload_same_cmd(payload, "Down", length) == true) {
      REMOTERK2_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK2_RELEASE();
      client.publish(topic_cb, "Down");
    }
    else if (is_payload_same_cmd(payload, "DownLong", length) == true) {
      REMOTERK4_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK4_RELEASE();
      delay(250);
      REMOTERK2_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK2_RELEASE();
      client.publish(topic_cb, "DownLong");
    }
    else if (is_payload_same_cmd(payload, "Lock", length) == true) {
      REMOTERK3_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK3_RELEASE();
      client.publish(topic_cb, "Lock");
    }
    else if (is_payload_same_cmd(payload, "Stop", length) == true) {
      REMOTERK4_PRESS();
      LEDG_OFF();
      delay(250);
      LEDG_ON();
      delay(250);
      REMOTERK4_RELEASE();
      client.publish(topic_cb, "Stop");
    }
  }
}

void setup() {
  uint8_t retry_count;

  LEDG_OFF();
  LEDB_OFF();
  pinMode(LEDG_PIN, OUTPUT);
  pinMode(LEDB_PIN, OUTPUT);

  pinMode(REMOTERK1_PIN, OUTPUT);
  pinMode(REMOTERK2_PIN, OUTPUT);
  pinMode(REMOTERK3_PIN, OUTPUT);
  pinMode(REMOTERK4_PIN, OUTPUT);
  REMOTERK1_RELEASE();
  REMOTERK2_RELEASE();
  REMOTERK3_RELEASE();
  REMOTERK4_RELEASE();

  retry_count = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    LEDB_ON();
    delay(200);
    LEDB_OFF();
    retry_count++;
    if (retry_count == 150) {
      retry_count = 0;
      ESP.restart();
    }
  }
  LEDG_ON();
  delay(1000);

  LEDB_ON();
  retry_count = 0;
  while (!client.connected()) {
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(mqtt_callback);
    client_id += String(WiFi.macAddress());
    if (!client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      delay(1000);
      retry_count++;
      if (retry_count == 20) {
        retry_count = 0;
        ESP.restart();
      }
    }
  }
  LEDB_OFF();

  client.subscribe(topic_sub);
}

void loop() {
  if ((WiFi.status() != WL_CONNECTED) || (!client.connected())) {
    ESP.restart();
  }
  client.loop();
}
