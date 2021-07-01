#ifndef _SENSOR_H
#define _SENSOR_H

#include "SensorReading.h"

class Sensor
{
  public:
    Sensor();
    Sensor(byte pin);
    virtual int read();
    virtual void display();
    virtual float parse();
    SensorReading reading();
    byte pin;
    int type;
    int value;
    int source;
    float value_parsed;
    bool analog;
    SensorReading last_reading;
  private:
    int _id;
};

#endif
