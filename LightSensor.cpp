#include "Arduino.h"
#include "system.h"
#include "LightSensor.h"

LightSensor::LightSensor()
{
  this->type = READING_TYPE_LIGHT;
  this->source = DEVICE_ID;
  this->value = 0;
  this->value_parsed = 0;
}
LightSensor::LightSensor(byte pin)
{
  this->pin = pin;
  this->analog = true;
  this->type = READING_TYPE_LIGHT;
  this->source = this->pin;
  this->value = 0;
  this->value_parsed = 0;
}
void LightSensor::display() {
  Serial.println("#####");
  Serial.println("Light Sensor");
  Sensor::display();
}
int LightSensor::read() {
  int intensity = 0;
  int intensity_avg = 0;
  int avg_count = 0;
  
  for (byte i = 0; i <= 10; i++) {
    if(i > 0) {
      intensity = analogRead(pin);
      intensity_avg += intensity;
      avg_count++;
    }
    else {
      //First reading after sleep is often garbage
      analogRead(pin);
    }
    delay(10);
  }
  intensity_avg = intensity_avg / avg_count;
  this->value = intensity_avg;
  return intensity_avg;
}
