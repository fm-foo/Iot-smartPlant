
#include "Adafruit_MQTT_Client.h"

/************************* Internal functions ********************************/
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect(Adafruit_MQTT_Client mqtt) {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
    return;

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0)
         while (1); // basically die and wait for WDT to reset me
  }

  Serial.println("MQTT Connected!");
}
