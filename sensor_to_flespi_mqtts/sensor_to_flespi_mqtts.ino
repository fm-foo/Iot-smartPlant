#include <WiFiManager.h> 
#include <Arduino_JSON.h>
#include "DHT.h"

/************************* MAIN DEFINITIONS *********************************/
#define ONEWIRE_PIN      14
#define STATIC_MESSAGE_BUF_LEN 256

#define MQTT_SERVER      "mqtt.flespi.io"
#define MQTT_SERVERPORT  8883
#define FLESPI_CERT_FINGERPRINT "6b4b7d8b78ecd7b7df253e969d5f1f9d3cb15157"
#define OUTPUT_TOPIC       "ESP8266/smartPlant"
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
// WifiManager for connecting to wifi
WiFiManager wm;
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


// thread handling
//Reading values
int readValuesOffTime = 5000;
unsigned long readValuesPreviousMillis = 0;

int level = 0; // level indicates which thread to do
unsigned long timeInterval = 5000;
unsigned long onTime = 5000; // time on activity 5seconds
const int led = 16; 
/*************************** Sketch Code ************************************/


void setup() {
  WiFi.mode(WIFI_STA);  
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  delay(3000);
  Serial.println("\n Starting");


  wm.setSaveParamsCallback(saveParamCallback);
  
  std::vector<const char *> menu = {"wifi","info","sep","restart","exit"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");

  wm.setConfigPortalTimeout(300); // auto close configportal after n seconds

  bool res;
  res = wm.autoConnect("SmartPlant");

  if(!res) {
    Serial.println("Failed to connect or hit timeout");
    ESP.restart();
  }
  else {
    // Connection established... proceed....
    Serial.println("connected...yeey :)");
    dht.begin();
    DisplaySetup();
    delay(10);
    
    client.setFingerprint(FLESPI_CERT_FINGERPRINT);

    pinMode(led, OUTPUT);
    digitalWrite(led, LOW); // keep the pump off at first
    mqtt.subscribe(&input);
  }
}

String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
}




void loop() {


  if(get_soil_moisture_value() <= 20){
      digitalWrite(led, HIGH); 
    }else if(get_soil_moisture_value() > 35){
      digitalWrite(led, LOW);
    }

  
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
