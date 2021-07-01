#include "PubSubClient.h"
#include "system.h"
#include <WiFiClient.h>


WiFiClient wifi_client;
PubSubClient client;
bool mqtt_connected;

bool setup_mqtt(char* server) {
	mqtt_connected = false;
	client.setClient(wifi_client);
	client.setServer(server, 1883);
}

bool connect_mqtt() {
	if (!mqtt_connected) {
    int attempts = 0;
    while (!client.connected() && attempts <= 3) {
      ++attempts;
      Serial.print("Connecting MQTT...");
      // Attempt to connect
      const char* client_name = "MudPi_" + DEVICE_ID;
      if (client.connect(client_name)) {
        Serial.println("Success");
        // Subscribe
        // client.subscribe(topic);
        mqtt_connected = true;
      } else {
        Serial.print("Failed, rc=");
        Serial.println(client.state());
        delay(500);
        mqtt_connected = false;
      }
    }
    if(!mqtt_connected) {
      Serial.println('MQTT Failed to Connect');
      return false;
    }
  
  }
  return true;
}

bool disconnect_mqtt() {
	client.disconnect(); //Disconnect mqtt
	return true;
}

bool send_mqtt(char* topic, String data) {
	Serial.print("Sending readings to MQTT server...");
	if(mqtt_connected) {    
	int send_attempts = 0;

	// Try to post the data up to three times
	bool sent_successfully = false;
	while(send_attempts < 3 && ! sent_successfully) {
	  sent_successfully = client.publish(topic, data.c_str());
    client.loop();
	  send_attempts++;

	  if(sent_successfully) {                     
	    Serial.println("Success! ");
      delay(500);
      client.loop();
	  }
	  else {
	    Serial.println("Failed");
	    delay(1000);
	  }
	}
	// Add failed request data to array
	if(!sent_successfully) {
	  Serial.println("Failed to Send to MQTT Server!");
	  return false;
	}
	return true;
	}
	return false;
}
