#include "Arduino.h"
#include "ReadingController.h"

#include "system.h"
//#include <ArduinoJson.h>

ReadingController::ReadingController()
{
  this->readingCount = 0;
}

bool ReadingController::queue_reading(SensorReading new_reading) {
  this->readings[this->readingCount] = new_reading;
  this->readingCount++;
  if (this->readingCount >= (MAX_SAVED_READINGS + FAILED_READINGS_LIMIT) ) {
    this->readingCount = (MAX_SAVED_READINGS + FAILED_READINGS_LIMIT);
  }
  return true;
}

DynamicJsonDocument ReadingController::to_json(int bootCount = 0) {
  // Format the reading to json.
  size_t capacity = JSON_ARRAY_SIZE(this->readingCount) + JSON_OBJECT_SIZE(3) + this->readingCount*JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);

  // StaticJsonDocument<500> doc;
  // Add values in the document
  doc["source"] = DEVICE_ID;
  doc["boot"] = bootCount;

  JsonArray _data;
  if( this->readingCount > 1 ) {
    // Add an array of readings.
    _data = doc.createNestedArray("data");
  }

  for(byte i = 0; i < this->readingCount; i++) {
    JsonObject data_0;
    if( this->readingCount > 1 ) {
      data_0 = _data.createNestedObject();
    }
    else {
      data_0 = doc.createNestedObject("data");
    }
    data_0["value"] = readings[i]._value;
    data_0["parsed"] = readings[i]._value_parsed;
    data_0["boot"] = readings[i]._boot;
    data_0["source"] = readings[i]._source;
    switch(readings[i]._type) {
      case READING_TYPE_VOLTAGE:
        data_0["type"] = "voltage";
        break;
      case READING_TYPE_SOIL:
        data_0["type"] = "soil";
        break;
      case READING_TYPE_TEMPERATURE:
        data_0["type"] = "temperature";
        break;
      case READING_TYPE_HUMIDITY:
        data_0["type"] = "humidity";
        break;
      case READING_TYPE_RAIN:
        data_0["type"] = "rain";
        break;
      case READING_TYPE_PRESSURE:
        data_0["type"] = "pressure";
        break;
      case READING_TYPE_FLOAT:
        data_0["type"] = "float";
        break;
      case READING_TYPE_PH:
        data_0["type"] = "ph";
        break;
      case READING_TYPE_EC:
        data_0["type"] = "ec";
        break;
      case READING_TYPE_LIGHT:
        data_0["type"] = "light";
        break;
      case READING_TYPE_HEATINDEX:
        data_0["type"] = "heat_index";
        break;
      default:
        data_0["type"] = "unknown";
        break;
    }
  }

  return doc;
  
}

DynamicJsonDocument ReadingController::reading_to_json(int bootCount = 0, int readingIndex = 0) {
  // Format the reading to json.
  size_t capacity = JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);

  // Add values in the document
  doc["current_boot"] = bootCount;
  doc["value"] = readings[readingIndex]._value;
  doc["parsed"] = readings[readingIndex]._value_parsed;
  doc["boot"] = readings[readingIndex]._boot;
  doc["source"] = readings[readingIndex]._source;
  switch(readings[readingIndex]._type) {
    case READING_TYPE_VOLTAGE:
      doc["type"] = "voltage";
      break;
    case READING_TYPE_SOIL:
      doc["type"] = "soil";
      break;
    case READING_TYPE_TEMPERATURE:
      doc["type"] = "temperature";
      break;
    case READING_TYPE_HUMIDITY:
      doc["type"] = "humidity";
      break;
    case READING_TYPE_RAIN:
      doc["type"] = "rain";
      break;
    case READING_TYPE_PRESSURE:
      doc["type"] = "pressure";
      break;
    case READING_TYPE_FLOAT:
      doc["type"] = "float";
      break;
    case READING_TYPE_PH:
      doc["type"] = "ph";
      break;
    case READING_TYPE_EC:
      doc["type"] = "ec";
      break;
    case READING_TYPE_LIGHT:
      doc["type"] = "light";
      break;
    case READING_TYPE_HEATINDEX:
      doc["type"] = "heat_index";
      break;
    default:
      doc["type"] = "unknown";
      break;
  }

  return doc;
  
}

DynamicJsonDocument ReadingController::config_to_json(int bootCount = 0, int totalReadings = 0, int failedReadings = 0) {
  // Send data about root device such as config and counts
  // Method put here since the JSON library is here (._.)
  size_t capacity = JSON_OBJECT_SIZE(9);
  DynamicJsonDocument doc(capacity);

  // Add values in the document
  doc["current_boot"] = bootCount;
  doc["total_readings"] = totalReadings;
  doc["failed_readings"] = failedReadings;
  doc["version"] = VERSION;
  doc["send_data_grouped"] = SEND_DATA_GROUPED;
  doc["source"] = DEVICE_ID;
  doc["ref_voltage"] = REF_VOLTAGE;
  doc["time_to_sleep"] = TIME_TO_SLEEP;
  doc["reset_on_fail"] = RESET_ON_FAIL;

  return doc;
  
}
