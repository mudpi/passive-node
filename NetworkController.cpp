#include "Arduino.h"
#include "NetworkController.h"

#include "system.h"
//#include "WiFi.h"
#include <ESPmDNS.h>
#include <HTTPClient.h>

NetworkController::NetworkController()
{
  this->connected = false;
}
NetworkController::NetworkController(char* ssid, char* password)
{
  this->ssid = ssid;
  this->password = password;
  this->connected = false;
  this->connected_mqtt = false;
}
NetworkController::NetworkController(char* ssid, char* password, char* endpoint)
{
  this->ssid = ssid;
  this->password = password;
  this->endpoint = endpoint;
  this->connected = false;
  this->connected_mqtt = false;
}
bool NetworkController::connect() {
  if (!this->connected) {
    int count = 0;
    int ip_count = 0;
    // it wil set the static IP address to 192, 168, 1, 1
    IPAddress null_IP(0, 0, 0, 0);
    IPAddress ip = null_IP;
    
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password); 
   // waitForConnectResult() != WL_CONNECTED;
    while (WiFi.waitForConnectResult() != WL_CONNECTED && ip == null_IP && count < CONNECTION_TRY_LIMIT) { //Check for the connection
      switch (WiFi.status()) {
        case WL_CONNECTED:
          Serial.println("Connection Success");
          break;
        case WL_IDLE_STATUS:
          Serial.println("Idle Waiting Connection");
          break;
        case WL_NO_SSID_AVAIL:
          Serial.println("No SSID Available");
          break;
        case WL_SCAN_COMPLETED:
          Serial.println("Scan Completed");
          break;
        case WL_CONNECT_FAILED:
          Serial.println("Connection Failed");
          break;
        case WL_CONNECTION_LOST:
          Serial.println("Connection Lost");
          break;
        case WL_DISCONNECTED:
          Serial.println("Connetion Disconnected");
          count = 4; // trigger wifi reset
          break;
      }
      ++count;
      
      ip = WiFi.localIP(); // this is used to update network
      if(count == 1) {
        Serial.print("Waiting for Connection...");
      }
      else {
  //      Serial.print(".");
        delay(300);
      }
  
      if(count % 5 == 0) {
        Serial.print("Reseting WiFi...");
        WiFi.disconnect();
        delay(2000);
        WiFi.begin(ssid, password); 
        delay(2000);
        ip = WiFi.localIP(); // used to update network
      }
    }
    
   // Check if WiFi was successful 
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to the WiFi network");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      this->connected = true;
      return true;
    }
    else {
      Serial.println("Failed to Connect to WiFi!");
      return false;
    }
  
  }
  return true;
}

bool NetworkController::disconnect() {
  WiFi.disconnect(); //Flush WiFi
  delay(300);
  Serial.println(WiFi.status());
  // this->client.disconnect(); //Disconnect mqtt
  return true;
}

bool NetworkController::setup() {
  WiFi.disconnect(); //Flush WiFi
  return true;
}

// bool NetworkController::setup_mqtt(char* server) {
//   WiFiClient wifi_client;
//   this->mqtt_server = server;
//   this->client.setClient(wifi_client);
//   this->client.setServer(server, 1883);
//   return true;
// }

bool NetworkController::send_http(String data) {
  Serial.println("Sending readings to server...");
  // Block until we are able to connect to the WiFi access point
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;   
    http.begin(this->endpoint);  
    http.addHeader("Content-Type", "application/json");      
    http.addHeader("Accept", "application/json");         

    int send_attempts = 0;

    // Try to post the data up to three times
    int httpResponseCode = 0;
    bool sent_successfully = false;
    while(send_attempts < 3 && httpResponseCode != 200) {
      httpResponseCode = http.POST(data);
      send_attempts++;
   
      if(httpResponseCode>0) {
        String response = http.getString();                       
        Serial.print("Response: ");
        Serial.println(httpResponseCode);   
        Serial.println(response);
        if (httpResponseCode == 200) {
            sent_successfully = true;
        }
      }
      else {
        Serial.printf("Error occurred while sending HTTP POST: %s\n", http.errorToString(httpResponseCode).c_str());
        delay(1500);
      }
    
    }
    // Add failed request data to array
    if(!sent_successfully) {
      Serial.println("Failed to Send to Database!");
      http.end();
      return false;
    }
    
    http.end();
    return true;
     
  }
  else {
    this->connected = false;
  }
}

// bool NetworkController::send_http(DynamicJsonDocument data) {
//   Serial.println("Sending readings to server...");
//   // Block until we are able to connect to the WiFi access point
//   if(WiFi.status() == WL_CONNECTED){
//     HTTPClient http;
//     http.begin(this->endpoint);  
//     http.addHeader("Content-Type", "application/json");      
//     http.addHeader("Accept", "application/json");         

//     //PACKAGE DATA
//     String requestBody;
//     serializeJson(data, requestBody);

//     int send_attempts = 0;

//     // Try to post the data up to three times
//     int httpResponseCode = 0;
//     bool sent_successfully = false;
//     while(send_attempts < 3 && httpResponseCode != 200) {
//       httpResponseCode = http.POST(requestBody);
//       send_attempts++;
   
//       if(httpResponseCode>0) {
//         String response = http.getString();                       
//         Serial.print("Response: ");
//         Serial.println(httpResponseCode);   
//         Serial.println(response);
//         if (httpResponseCode == 200) {
//             sent_successfully = true;
//         }
//       }
//       else {
//         Serial.printf("Error occurred while sending HTTP POST: %s\n", http.errorToString(httpResponseCode).c_str());
//         delay(1500);
//       }
    
//     }
//     // Add failed request data to array
//     if(!sent_successfully) {
//       Serial.println("Failed to Send to Database!");
//       http.end();
//       return false;
//     }
    
//     http.end();
//     return true;
     
//   }
//   else {
//     this->connected = false;
//   }
// }

// bool NetworkController::connect_mqtt() {
//   if (!this->connected_mqtt) {
//     int attempts = 0;
//     while (!this->client.connected() && attempts <= 10) {
//       ++attempts;
//       Serial.print("Connecting MQTT...");
//       // Attempt to connect
//       const char* client_name = "MudPi_" + DEVICE_ID;
//       if (this->client.connect(client_name)) {
//         Serial.println("Success");
//         // Subscribe
//         // this->client.subscribe(this->topic);
//         this->connected_mqtt = true;
//       } else {
//         Serial.print("Failed, rc=");
//         Serial.println(this->client.state());
//         delay(500);
//         this->connected_mqtt = false;
//       }
//     }
//     if(!this->connected_mqtt) {
//       Serial.println('MQTT Failed to Connect');
//       return false;
//     }
  
//   }
//   return true;
// }

// bool NetworkController::send_mqtt(char* topic, String data) {
  // Serial.print("Sending readings to MQTT server...");
  // if(this->connected_mqtt) {    
  //   int send_attempts = 0;

  //   // Try to post the data up to three times
  //   bool sent_successfully = false;
  //   while(send_attempts < 3 && ! sent_successfully) {
  //     sent_successfully = this->client.publish(topic, data.c_str());
  //     send_attempts++;
   
  //     if(sent_successfully) {                     
  //       Serial.println("Success! ");
  //     }
  //     else {
  //       Serial.println("Failed");
  //       delay(1000);
  //     }
  //   }
  //   // Add failed request data to array
  //   if(!sent_successfully) {
  //     Serial.println("Failed to Send to MQTT Server!");
  //     return false;
  //   }
  //   return true;
  // }
  // return false;
//}

// bool NetworkController::send_mqtt(char* topic, DynamicJsonDocument data) {
//   Serial.print("Sending readings to MQTT server...");
//   if(this->connected_mqtt) {    
//     int send_attempts = 0;

//     // Try to post the data up to three times
//     bool sent_successfully = false;
//     //PACKAGE DATA
//     char* jsonString;
//     serializeJson(data, jsonString);
//     while(send_attempts < 3 && ! sent_successfully) {
//       sent_successfully = this->client.publish(topic, jsonString);
//       send_attempts++;
   
//       if(sent_successfully) {                     
//         Serial.println("Success! ");
//       }
//       else {
//         Serial.println("Failed");
//         delay(1000);
//       }
//     }
//     // Add failed request data to array
//     if(!sent_successfully) {
//       Serial.println("Failed to Send to MQTT Server!");
//       return false;
//     }
//     return true;
//   }
//   return false;
// }
