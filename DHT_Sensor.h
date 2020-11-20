#include "DHT.h"

#define dhtPin 2
#define dhtType DHT22
DHT dht(dhtPin , dhtType);

float humidityVal;
float tempValC;


float get_humidity_value() {
  humidityVal = dht.readHumidity();

  if (isnan(humidityVal)){
    Serial.println("Reading DHT sensor failed");
    delay(2000);
    return -1;
    }
  return humidityVal; // data written to passed variable
}

float get_temperature_value() {
  tempValC = dht.readTemperature();

  if (isnan(tempValC)){
    Serial.println("Reading DHT sensor failed");
    delay(2000);
    return -1;
    }
  return tempValC; // data written to passed variable
}
