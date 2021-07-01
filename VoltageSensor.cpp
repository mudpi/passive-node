#include "Arduino.h"
#include <esp_adc_cal.h>
#include "system.h"
#include "VoltageSensor.h"

VoltageSensor::VoltageSensor()
{
  this->atten = ADC_ATTEN_DB_0;
  this->battery_min = BATTERY_MIN;
  this->battery_max = BATTERY_MAX;
  this->type = READING_TYPE_VOLTAGE;
  this->source = 0;
  this->value = 0;
  this->value_parsed = 0.0f;
}
VoltageSensor::VoltageSensor(byte pin)
{
  this->pin = pin;
  this->analog = true;
  this->atten = ADC_ATTEN_DB_0;
  this->battery_min = BATTERY_MIN;
  this->battery_max = BATTERY_MAX;
  this->type = READING_TYPE_VOLTAGE;
  this->source = this->pin;
  this->value = 0;
  this->value_parsed = 0.0f;
}
VoltageSensor::VoltageSensor(byte pin, esp_adc_cal_characteristics_t *adc_chars)
{
  this->pin = pin;
  this->analog = true;
  this->atten = ADC_ATTEN_DB_0;
  this->adc_chars = adc_chars;
  this->battery_min = BATTERY_MIN;
  this->battery_max = BATTERY_MAX;
  this->type = READING_TYPE_VOLTAGE;
  this->source = this->pin;
  this->value = 0;
  this->value_parsed = 0.0f;
}
void VoltageSensor::display() {
  Serial.println("#####");
  Serial.println("Voltage Sensor");
  Sensor::display();
}

int VoltageSensor::read() {
  //Read voltage a few times as the first reading after sleep result is often garbage
  for (byte i = 0; i < 2; i++) {
      adc1_channel_t channelNum;

      //Set number of cycles per sample 1 - 255
      analogSetCycles(16);
    
      switch (this->pin) {
        case (36):
          channelNum = ADC1_CHANNEL_0;
          break;
        case (39):
          channelNum = ADC1_CHANNEL_3;
          break;
        case (34):
          channelNum = ADC1_CHANNEL_6;
          break;
        case (32):
          channelNum = ADC1_CHANNEL_4;
          break;
        case (33):
          channelNum = ADC1_CHANNEL_5;
          break;
        case (27):
          channelNum = ADC1_CHANNEL_7;
          break;
      }
    
      adc1_config_channel_atten(channelNum, this->atten);
      this->value = esp_adc_cal_raw_to_voltage(analogRead(pin), adc_chars);
      this->value = this->value / (R2 / (R1 + R2)); //reverse voltage divider
      delay(2000);
  }
  
  VoltageSensor::parse();
  return this->value;
  
}

float VoltageSensor::parse() {
  this->value_parsed = (((float)this->value - (float)this->battery_min) / ((float)this->battery_max - (float)this->battery_min)) * 100;
  this->value_parsed = roundf(this->value_parsed  * 100) / 100;
  if (this->value_parsed > 100) {
    this->value_parsed = 100.0f;
  }
  return this->value_parsed ;
}
