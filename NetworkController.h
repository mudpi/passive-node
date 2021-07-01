#ifndef _NETWORKCONTROLLER_H
#define _NETWORKCONTROLLER_H

// #include <ArduinoJson.h>


class NetworkController
{
  public:
    NetworkController();
    NetworkController(char* ssid, char* password);
    NetworkController(char* ssid, char* password, char* endpoint);
    bool connect();
    // bool connect_mqtt();
    bool disconnect();
    bool setup();
    // bool setup_mqtt(char* server);
    bool send_http(String data);
    // bool send_http(DynamicJsonDocument data);
    // bool send_mqtt(char* topic, String data);
    // bool send_mqtt(char* topic, DynamicJsonDocument data);
    char* ssid;
    char* password;
    char* endpoint;
    char* mqtt_server;
    bool connected;
    bool connected_mqtt;
  private:
    int _id;
};

#endif
