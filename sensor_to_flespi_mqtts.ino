#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "DHT.h"

/************************* MAIN DEFINITIONS *********************************/
#define ONEWIRE_PIN      14
#define STATIC_MESSAGE_BUF_LEN 256

#define MQTT_SERVER      "mqtt.flespi.io"
#define MQTT_SERVERPORT  8883
#define FLESPI_CERT_FINGERPRINT "6b4b7d8b78ecd7b7df253e969d5f1f9d3cb15157"
#define OUTPUT_TOPIC       "ESP8266/sensors"
#define INPUT_TOPIC      "ESP8266/input"
#define FLESPI_TOKEN    "FlespiToken pUgcWLHBBPPcrVYPwXB8tXC8L4fTZAFejJg5lLxKoaMFWyjglIeEvEB7wEOl4jpd"

#define WLAN_SSID       "ItBurnsWhenIP"
#define WLAN_PASS       "e1ce4c8a8ad6"

/**************************** Local files ***********************************/
#include "DHT_Sensor.h"
#include "adafruit_mqtts.h"
#include "soil_moisture_sensor.h"
/********************* Global connection instances **************************/

// WiFiFlientSecure for SSL/TLS support
WiFiClientSecure client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, FLESPI_TOKEN, "");
// Setup a feed 'flespi' to publish messages to flespi MQTT broker.
Adafruit_MQTT_Publish flespi = Adafruit_MQTT_Publish(&mqtt, OUTPUT_TOPIC);
Adafruit_MQTT_Subscribe input = Adafruit_MQTT_Subscribe(&mqtt, INPUT_TOPIC);
// JSON message creation part
JSONVar message_object;         // to store message parameters
String json_msg_string;         // to stringify JSON message object
char message_string_buf[STATIC_MESSAGE_BUF_LEN];   // to use in mqtt publish function

// variables for testing
int time_value = 5000;

/*************************** Sketch Code ************************************/
// Generic example for ESP8266 wifi connection
void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);

  // Connect to WiFi access point.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  delay(1000);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  client.setFingerprint(FLESPI_CERT_FINGERPRINT);

  mqtt.subscribe(&input);
  
}

void loop() {
  
  message_object["soil_humidity"] = get_soil_moisture_value();
  message_object["humidity"] = get_humidity_value();
  message_object["temperature_c"] = get_temperature_value();

  // establish MQTT connection
  MQTT_connect(mqtt);

  // send data to flespi MQTT broker via secure connection
  json_msg_string = JSON.stringify(message_object);
  json_msg_string.toCharArray(message_string_buf, json_msg_string.length() + 1);
  Serial.print("Publishing message to broker: ");
  Serial.println(message_string_buf);
  flespi.publish(message_string_buf);

  // cleanup memory used
  memset(message_string_buf, 0, STATIC_MESSAGE_BUF_LEN);

  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(time_value))) {
    if (subscription == &input) {
      Serial.print(F("changed time intervals to "));
      uint16_t ledBrightValue = atoi((char *)input.lastread);
      Serial.print(ledBrightValue);
      Serial.println(" miliSeconds");
      time_value = ledBrightValue;
      Serial.println(time_value);
    }
  }
}
