#include "Arduino.h"
#include "SensorReading.h"

SensorReading::SensorReading()
{}
SensorReading::SensorReading(int type)
{
  this->_type = type;
  this->_value = 0;
  this->_source = 0;
  this->_value_parsed = 0;
  this->_boot = 1;
}
SensorReading::SensorReading(int type, int value)
{
  this->_type = type;
  this->_value = value;
  this->_source = 0;
  this->_value_parsed = 0;
  this->_boot = 1;
}
SensorReading::SensorReading(int type, int value, int source)
{
  this->_type = type;
  this->_value = value;
  this->_source = source;
  this->_value_parsed = 0;
  this->_boot = 1;
}
SensorReading::SensorReading(int type, int value, int source, float value_parsed)
{
  this->_type = type;
  this->_value = value;
  this->_source = source;
  this->_value_parsed = value_parsed;
  this->_boot = 1;
}
SensorReading::SensorReading(int type, int value, int source, float value_parsed, int boot)
{
  this->_type = type;
  this->_value = value;
  this->_source = source;
  this->_value_parsed = value_parsed;
  this->_boot = boot;
}
void SensorReading::display() {
  Serial.println("#####");
  Serial.println("Sensor Reading");
  Serial.print("Type: ");
  Serial.println(this->_type);
  Serial.print("Value: ");
  Serial.println(this->_value);
  Serial.print("Value Parsed: ");
  Serial.println(this->_value_parsed);
  Serial.print("Source: sprout-");
  Serial.println(this->_source);
  Serial.print("Boot: ");
  Serial.println(this->_boot);
  Serial.println("#####");
}
