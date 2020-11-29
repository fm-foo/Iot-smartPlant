#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include "DHT.h"

/************************* MAIN DEFINITIONS *********************************/
#define ONEWIRE_PIN      14
#define STATIC_MESSAGE_BUF_LEN 256

#define MQTT_SERVER      "mqtt.flespi.io"
#define MQTT_SERVERPORT  8883
#define FLESPI_CERT_FINGERPRINT "6b4b7d8b78ecd7b7df253e969d5f1f9d3cb15157"
#define OUTPUT_TOPIC       "ESP8266/sensor"
#define INPUT_TOPIC      "ESP8266/input"
#define FLESPI_TOKEN    "FlespiToken pUgcWLHBBPPcrVYPwXB8tXC8L4fTZAFejJg5lLxKoaMFWyjglIeEvEB7wEOl4jpd"

#define WLAN_SSID       "ItBurnsWhenIP"
#define WLAN_PASS       "e1ce4c8a8ad6"

/**************************** Local files ***********************************/
#include "DHT_Sensor.h"
#include "adafruit_mqtts.h"
#include "soil_moisture_sensor.h"
#include "SH1106Display.h"
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

String device_id = "87761e13-d509-4aa4-8dca-6e0915f6645b";
int SoilMoistureValue = 0; 
int HumidityValue = 0;
int TemperatureValue = 0;


// thead handling
  //Reading values
int readValuesOffTime = 5000;
unsigned long readValuesPreviousMillis = 0;


/*************************** Sketch Code ************************************/
// Generic example for ESP8266 wifi connection
void setup() {
  Serial.begin(115200);
  dht.begin();
  DisplaySetup();
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

int level = 0;
unsigned long timeInterval = 5000;

unsigned long onTime = 5000;

void loop() {

  unsigned long currentMillis = millis();
  
  if((level == 0)){
    animateHappy(currentMillis);
    if(currentMillis >= onTime){
      onTime = currentMillis + timeInterval;
      animateHappy(currentMillis);
      level = 1;
    }
  }else if((level == 1)){
    animateSoilMoistureProgressBar(currentMillis, SoilMoistureValue);
    if(currentMillis >= onTime){
      onTime = currentMillis + timeInterval;
      animateSoilMoistureProgressBar(currentMillis, SoilMoistureValue);
      level = 2;
    }
  }else if((level == 2)){
    animateTeperatureProgressBar(currentMillis, TemperatureValue);
    if(currentMillis >= onTime){
      onTime = currentMillis + timeInterval;
      animateTeperatureProgressBar(currentMillis, TemperatureValue);
      level = 3;
    }
  }else if((level == 3)){
    animateHumidityProgressBar(currentMillis, HumidityValue);
    if(currentMillis >= onTime){
      onTime = currentMillis + timeInterval;
      animateTeperatureProgressBar(currentMillis, HumidityValue);
      level = 0;
    }
  }
  
  
  
  if (currentMillis - readValuesPreviousMillis >= readValuesOffTime) {
    readValuesPreviousMillis = currentMillis;

    SoilMoistureValue = get_soil_moisture_value();
    HumidityValue = get_humidity_value();
    TemperatureValue = get_temperature_value();

    message_object["soil_humidity"] = SoilMoistureValue;
    message_object["humidity"] = HumidityValue;
    message_object["temperature_c"] = TemperatureValue;
    message_object["device_id"] = device_id;
    
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
  }


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(10))) {
    if (subscription == &input) {
      Serial.print(F("changed time intervals to "));
      uint16_t ledBrightValue = atoi((char *)input.lastread);
      Serial.print(ledBrightValue);
      Serial.println(" miliSeconds");
      readValuesOffTime = ledBrightValue;
      Serial.println(readValuesOffTime);
    }
  }
}
