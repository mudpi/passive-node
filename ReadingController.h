#ifndef _READINGCONTROLLER_H
#define _READINGCONTROLLER_H

#include "SensorReading.h"
#include <ArduinoJson.h>

class ReadingController
{
  public:
    ReadingController();
    DynamicJsonDocument to_json(int bootCount);
    DynamicJsonDocument reading_to_json(int bootCount, int readingIndex);
    DynamicJsonDocument config_to_json(int bootCount, int totalReadings, int failedReadings);
    bool queue_reading(SensorReading reading);
    SensorReading readings[20];
    int readingCount;
  private:
    int _id;
};

#endif
