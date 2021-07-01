#ifndef VoltageSensor_h
#define VoltageSensor_h

#include "Sensor.h"

class VoltageSensor : public Sensor
{
  public:
    VoltageSensor();
    VoltageSensor(byte pin);
    VoltageSensor(byte pin, esp_adc_cal_characteristics_t *adc_chars);
    void display();
    int read();
    float parse();
    adc_atten_t atten;
    esp_adc_cal_characteristics_t *adc_chars;
    int battery_min;
    int battery_max;
    int source;
};
#endif
