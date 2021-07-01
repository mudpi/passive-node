#include "Arduino.h"
#include "system.h"
#include "SoilSensor.h"

SoilSensor::SoilSensor()
{
  this->type = READING_TYPE_SOIL;
  this->source = DEVICE_ID;
  this->value = 0;
  this->value_parsed = 0;
}
SoilSensor::SoilSensor(byte pin)
{
  this->pin = pin;
  this->analog = true;
  this->type = READING_TYPE_SOIL;
  this->source = this->pin;
  this->value = 0;
  this->value_parsed = 0;
}
void SoilSensor::display() {
  Serial.println("#####");
  Serial.println("Soil Sensor");
  Sensor::display();
}
int SoilSensor::read() {
  int moisture = 0;
  int moisture_avg = 0;
  int avg_count = 0;
  
  for (byte i = 0; i <= 10; i++) {
    if(i > 0) {
      moisture = analogRead(pin);
      moisture_avg += moisture;
      avg_count++;
    }
    else {
      //First reading after sleep is often garbage
      analogRead(pin);
    }
    delay(10);
  }
  moisture_avg = moisture_avg / avg_count;
  this->value = moisture_avg;
  return moisture_avg;
}
