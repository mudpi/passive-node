#include "config.h"
#include "system.h"

#include <esp_adc_cal.h>
#include <esp32-hal-cpu.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Sensor.h"
#include "SensorReading.h"
#include "VoltageSensor.h"
#include "SoilSensor.h"
#include "LightSensor.h"
#include "NetworkController.h"
#include "ReadingController.h"
#include "deep_sleep.h"
#include "watchdog.h"
#include "mqtt.h"
#include "helpers.h"
#if DHT_PIN != NULL
#include "DHT.h"
#define DHT_MODEL DHT11
#endif

//////////////////////////////////////////
// Sprout Passive Node Firmware v0.8.0
//////////////////////////////////////////
// A ESP32 firmware to monitor battery voltage and take sensor readings.
// The unit is powered off a battery with solar charging and spends most
// of the time in deep sleep. Change the settings below based on components. 
// Author: Eric Davisson
// Website: MudPi.app
////////////////////////////////////////////
// Edit the settings in the config.h file //


/////////////////////////////////////////////////////////
// Do not change these unless you know what you are doing!
////////////////////////////////////////////////////


////////////////////////////////////////
// SYSTEM VARIABLES
int last_value = 0;
int sensorCount = 0;
unsigned long runtime;

// used for adc calibration
esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;

NetworkController network;
ReadingController reading_manager;

//////////////////////////
// Deep Sleep Data 
//Variable to survive deep sleep
RTC_NOINIT_ATTR int bootCount = 0;
RTC_NOINIT_ATTR int totalReadingsCount = 0;
RTC_NOINIT_ATTR int failedReadingsIndex = 0;
RTC_NOINIT_ATTR int failedReadingsCount = 0;

typedef struct {
    int type; // Sensor Reading Type
    byte source; // Node Id
    int value;
    int boot;
    float value_parsed;
} reading_t;

RTC_DATA_ATTR reading_t failed_readings[FAILED_READINGS_LIMIT];

/////////////////////////////////////
// Sensors
int readingsCount = 0;
Sensor *sensors[TOTAL_SENSORS];

SensorReading last_reading;
SensorReading readings[MAX_SAVED_READINGS];


////////////////////////
// Data Controls
void print_reading(reading_t reading) {
  Serial.print("Name: Sprout-");
  Serial.println(reading.source);
  Serial.print("Type: ");
  Serial.println(reading.type);
  Serial.print("Value: ");
  Serial.println(reading.value);
  Serial.print("Boot: ");
  Serial.println(reading.boot);
}

#if DHT_PIN != NULL
DHT dht(DHT_PIN, DHT_MODEL);
#endif

////////////////////////
// Main Program Loop
//////////////////
void setup() {
  Serial.begin(115200);
  // delay(1000);
//  rtc_watchdog_enable();
  // delay(100);
  
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset Reason: ");
  Serial.println(reason);
  readingsCount = 0;
  if ((reason != ESP_RST_DEEPSLEEP) && (reason != ESP_RST_SW)) {
    bootCount = 0;
    totalReadingsCount = 0;
    failedReadingsIndex = 0;
    failedReadingsCount = 0;
  }
  // Corrupt memory check
  if (bootCount < 0 || bootCount > 9999999) {
    bootCount = 0;
    totalReadingsCount = 0;
    failedReadingsIndex = 0;
    failedReadingsCount = 0;
    ESP.restart();
  }
  Serial.println("");
  Serial.println("----------");
  Serial.println("Sprout Booting...");
  Serial.print("Sprout ID: ");
  Serial.println(DEVICE_ID);

  setCpuFrequencyMhz(80);

  // ADC Calibration function
  analogReadResolution(11);
  esp_adc_cal_value_t val_type =
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
  
  ++bootCount;
  Serial.println("Boot cycle: " + String(bootCount));
  Serial.println("Failed Reading current index: " + String(failedReadingsIndex));
  Serial.println("Total Failed Reading count: " + String(failedReadingsCount));
  Serial.println("Total Readings count: " + String(totalReadingsCount));
  Serial.println("");
  

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  //disable brownout detector
  if (DISABLE_BROWNOUT) {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  }
  
  ////////////////////////////////////////////
  // Initialize the Sensors
  for(byte i = 0; i < TOTAL_SENSORS_VOLTAGE; i++) {
    sensors[sensorCount] = new VoltageSensor(voltage_sensor_pins[i], adc_chars);
    sensorCount++;
  }
  
  for(byte i = 0; i < TOTAL_SENSORS_SOIL; i++) {
    sensors[sensorCount] = new SoilSensor(soil_sensor_pins[i]);
    sensorCount++;
  }

  for(byte i = 0; i < TOTAL_SENSORS_LIGHT; i++) {
    sensors[sensorCount] = new LightSensor(light_sensor_pins[i]);
    sensorCount++;
  }


  
  // Prepare sensor enable pin
  pinMode(sensor_enable_pin, OUTPUT);


  network = NetworkController(ssid, password, server_url);
  reading_manager = ReadingController();
  
  if(network.setup()) {
    Serial.println("Network Controller Online");

    // Check to enable mqtt
    if(!server_mqtt == NULL) {
      if(setup_mqtt(server_mqtt)) {
        Serial.println("MQTT Online");
      }
    }
  }

//  delay(2000);
}

// Loop only runs once due to sleep
void loop() {
  bool sent_successfully = false;

  //////////////////////////////////////////////
  // FAILED DATA RESEND
  // Check for failed readings that need resending
  if(failedReadingsCount > 0) {
    int bootCycle = 0;
    for(byte i; i < failedReadingsCount; i++) {
      
      byte currentIndex = failedReadingsIndex + i;
      if (currentIndex > (FAILED_READINGS_LIMIT - 1)) {
        currentIndex = i - failedReadingsIndex;
      }

      if (bootCycle == 0) {
        bootCycle = failed_readings[currentIndex].boot;
      }

      // Detect new batch of failed readings
      if (bootCycle != failed_readings[currentIndex].boot) {
        // useful for future
      }

      // Corrupt bounds checking on failed deep sleep data
      if (failed_readings[currentIndex].value_parsed > 99999 || 
        failed_readings[currentIndex].value_parsed < -9999 ||
        failed_readings[currentIndex].value > 99999 || 
        failed_readings[currentIndex].value < -9999 ||
        failed_readings[currentIndex].boot > 99999 || 
        failed_readings[currentIndex].boot < -9999) {
        continue;
      }
      
      last_reading = SensorReading(failed_readings[currentIndex].type, failed_readings[currentIndex].value, failed_readings[currentIndex].source, failed_readings[currentIndex].value_parsed, failed_readings[currentIndex].boot);
      
      reading_manager.queue_reading(last_reading);

      if (!SEND_DATA_GROUPED) {
        if(server_mqtt != NULL) {
          if(network.connect()) {
            if(connect_mqtt()) {
              String mqtt_data;
              serializeJson(reading_manager.to_json(bootCount), mqtt_data);
              String _topic = String(mqtt_topic) + "/" + get_sensor_type(failed_readings[currentIndex].type) + "/" + String(failed_readings[currentIndex].source);
              char *topic = const_cast<char*>(_topic.c_str());
              if(send_mqtt(topic, mqtt_data)) {
                Serial.print("Failed Reading resent over MQTT to ");
                Serial.println(topic);
                failedReadingsCount = 0;
                failedReadingsIndex = 0;
                reading_manager.readingCount = 0;
                sent_successfully = true;
              }
            }
          }
        }
      }

    }

    // Connect to Wifi and services
    if(network.connect()) {
      if(server_mqtt != NULL && SEND_DATA_GROUPED) {
        if(connect_mqtt()) {
          String mqtt_data;
          serializeJson(reading_manager.to_json(bootCount), mqtt_data);
          if(send_mqtt(mqtt_topic, mqtt_data)) {
            Serial.println("Failed Data resent over MQTT successfully");
            failedReadingsCount = 0;
            failedReadingsIndex = 0;
            reading_manager.readingCount = 0;
            sent_successfully = true;
          }
        }
      }
      
      if(server_url != NULL) {
        String data;
        serializeJson(reading_manager.to_json(bootCount), data);
        if(network.send_http(data)) {
          Serial.println("Failed Data sent over HTTP successfully");
          sent_successfully = true;
          reading_manager.readingCount = 0;
    //      rtc_watchdog_pet();
        }
      }

      network.disconnect();
    }
//    delay(500);
  }
  /////////////////////////////////////////////


  //////////////////////////////////////
  // READ SENSOR DATA
  // Enable the sensors
  digitalWrite(sensor_enable_pin, HIGH);
  delay(100);
  
  // Sensor Read Loop
  for(byte i = 0; i < TOTAL_SENSORS; i++) {
    // sensors[i]->display();
    Serial.print("Sensor Reading: ");
    Serial.println(sensors[i]->read());
    
    last_reading = sensors[i]->reading();
    last_reading._boot = bootCount;
    // last_reading.display();
    
    readings[readingsCount] = last_reading;
    readingsCount++;
    totalReadingsCount++;
    if (readingsCount >= MAX_SAVED_READINGS) {
      readingsCount = MAX_SAVED_READINGS;
    }

    reading_manager.queue_reading(last_reading);
//    delay(500);
  }

  // Because DHT Library is not friendly
  // we must manually do reads ....
  #if DHT_PIN != NULL
  dht.begin();
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(DHT_FAHRENHEIT);
  if (!isnan(humidity) && !isnan(temperature)) {
    float heat_index = dht.computeHeatIndex(temperature, humidity, DHT_FAHRENHEIT);
    last_reading = SensorReading(READING_TYPE_HUMIDITY, humidity, DHT_PIN, humidity, bootCount);
    readings[readingsCount] = last_reading;
    readingsCount++;
    totalReadingsCount++;
    reading_manager.queue_reading(last_reading);
    
    last_reading = SensorReading(READING_TYPE_TEMPERATURE, temperature, DHT_PIN, temperature, bootCount);
    readings[readingsCount] = last_reading;
    readingsCount++;
    totalReadingsCount++;
    reading_manager.queue_reading(last_reading);
    
    last_reading = SensorReading(READING_TYPE_HEATINDEX, heat_index, DHT_PIN, heat_index, bootCount);
    readings[readingsCount] = last_reading;
    readingsCount++;
    totalReadingsCount++;
    reading_manager.queue_reading(last_reading);
  }
  
  #endif

  
  // Disable the sensors
  digitalWrite(sensor_enable_pin, LOW);

  delay(500); // wait for power to stablize
  
  if(network.connect()) {

    // Check for mqtt data send
    if (!SEND_DATA_GROUPED) {
      if(server_mqtt != NULL) {
        if(connect_mqtt()) {
          for(byte i = 0; i < readingsCount; i++) {
            String mqtt_data;
            serializeJson(reading_manager.reading_to_json(bootCount, i), mqtt_data);
            String _topic = String(mqtt_topic) + "/" + get_sensor_type(readings[i]._type) + "/" + String(readings[i]._source);
            char *topic = const_cast<char*>(_topic.c_str());
            if(send_mqtt(topic, mqtt_data)) {
              Serial.print("Reading sent to ");
              Serial.println(topic);
              sent_successfully = true;
            }
            else {
              sent_successfully = false;
            }
          }

          String mqtt_data;
          serializeJson(reading_manager.config_to_json(bootCount, totalReadingsCount, failedReadingsCount), mqtt_data);
          String _topic = String(mqtt_topic);
          char *topic = const_cast<char*>(_topic.c_str());
          if(send_mqtt(topic, mqtt_data)) {
            Serial.print("Device info sent to ");
            Serial.println(topic);
          }

        }
      }
    }
    else if(server_mqtt != NULL && SEND_DATA_GROUPED) {
      if(connect_mqtt()) {
        String data;
        serializeJson(reading_manager.to_json(bootCount), data);
        if(send_mqtt(mqtt_topic, data)) {
          Serial.println("Data sent over MQTT successfully");
          sent_successfully = true;
    //      rtc_watchdog_pet();
        }
      }
    }
    else if(server_url != NULL) {
      if(network.connect()) {
        String data;
        serializeJson(reading_manager.to_json(bootCount), data);
        if(network.send_http(data)) {
          Serial.println("Data sent over HTTP successfully");
          sent_successfully = true;
    //      rtc_watchdog_pet();
        }
      }
    }

  }


  /////////////////////////////////////////////
  //Deep Sleep Sensor Failed Reading Logs
  if(!sent_successfully) {
    Serial.println("Data Failed to Send to MudPi!");

    for(byte i = 0; i <= readingsCount; i++) {
      reading_t failed_reading;
      
      failed_reading.value = readings[i]._value;
      failed_reading.value_parsed = readings[i]._value_parsed;
      failed_reading.type = readings[i]._type;
      failed_reading.boot = readings[i]._boot;
      failed_reading.source = readings[i]._source;
      
      failed_readings[failedReadingsIndex] = failed_reading;
      
      //print_reading(failed_reading);
      
      failedReadingsCount++;
      failedReadingsIndex++;
      
      if (failedReadingsIndex >= FAILED_READINGS_LIMIT - 1) {
        failedReadingsIndex = 0; // Loop to the start of array and overwrite oldest data first
      }
      if (failedReadingsCount >= FAILED_READINGS_LIMIT - 1) {
        failedReadingsCount = FAILED_READINGS_LIMIT - 1;
      }
    }

    network.disconnect();
    if (RESET_ON_FAIL) {
      ESP.restart();
    }
    else {
      enter_deep_sleep(TIME_TO_SLEEP_ON_FAIL);
    }
  }
  /////////////////////////////////////////
  
  
  if(sent_successfully) {
    reading_manager.readingCount = 0;
  }
  network.disconnect();
  Serial.println("----------");
  Serial.print("Execution Time: ");
  runtime = millis();
  Serial.print((runtime/1000));
  Serial.println("s");
  enter_deep_sleep(TIME_TO_SLEEP);
}
