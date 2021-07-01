#ifndef LightSensor_h
#define LightSensor_h

#include "Sensor.h"

class LightSensor : public Sensor 
{
  public:
    LightSensor();
    LightSensor(byte pin);
    void display();
    int read();
    int source;
};
#endif
