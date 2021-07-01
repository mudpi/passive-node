#include "Arduino.h"
#include "Sensor.h"

Sensor::Sensor()
{
  this->analog = false;
  this->type = 0;
  this->source = 0;
}
Sensor::Sensor(byte pin)
{
  this->pin = pin;
  this->analog = false;
  this->type = 0;
  this->source = this->pin;
}
void Sensor::display() {
  Serial.println("#####");
  Serial.println("Sensor");
  Serial.print("Pin: ");
  Serial.println(this->pin);
  Serial.print("Analog: ");
  Serial.println(this->analog);
  Serial.print("Source: ");
  Serial.println(this->source);
}
int Sensor::read() {
  Serial.println("Reading Pin Sensor...");
  if(this->analog) {
    this->value = analogRead(this->pin);
    return this->value;
  }
  else {
    this->value = digitalRead(this->pin);
    return this->value;
  }
}
SensorReading Sensor::reading() {
  this->last_reading = SensorReading(this->type, this->value, this->pin, this->value_parsed);
  return this->last_reading;
}
float Sensor::parse() {
  return this->value_parsed;
}
