//////////////////////////////////////////////////////
// CONFIGURATION FILE
// Change the configurations below depending on your specific node setup.


// Soil Moisture Settings
// Pins to read moisture on
#define TOTAL_SENSORS_SOIL  1
byte soil_sensor_pins[TOTAL_SENSORS_SOIL] = {33};

// Battery Voltage Sensor Settings
// Pins to read battery voltage on
#define TOTAL_SENSORS_VOLTAGE  1
byte voltage_sensor_pins[TOTAL_SENSORS_VOLTAGE] = {36};

// Light Sensor Settings
// Pins to read light intensity on
#define TOTAL_SENSORS_LIGHT  1
byte light_sensor_pins[TOTAL_SENSORS_LIGHT] = {32};

// DHT Sensor Settings (NOT FULLY IMPLMENTED)
// Only 1 DHT supported. Model can be changed in main file.
#define DHT_PIN 26
#define DHT_FAHRENHEIT true

// Connect sensors PWR to this pin
// so they can be toggled off to save power
byte sensor_enable_pin = 25;

// Wifi Settings
char* ssid = "";
char* password = "";
char* server_url = NULL; //"http://192.168.2.64:8080/states/store";
char* server_mqtt = "192.168.2.64";
char* mqtt_topic = "mudpi/sprout/7";

// Helper Count
#define TOTAL_SENSORS TOTAL_SENSORS_SOIL + TOTAL_SENSORS_VOLTAGE + TOTAL_SENSORS_LIGHT
