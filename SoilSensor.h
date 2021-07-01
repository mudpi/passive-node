#ifndef SoilSensor_h
#define SoilSensor_h

#include "Sensor.h"

class SoilSensor : public Sensor 
{
  public:
    SoilSensor();
    SoilSensor(byte pin);
    void display();
    int read();
    int source;
};
#endif
