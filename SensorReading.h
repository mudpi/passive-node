#ifndef SensorReading_h
#define SensorReading_h

class SensorReading
{
  public:
    SensorReading();
    SensorReading(int type);
    SensorReading(int type, int value);
    SensorReading(int type, int value, int source);
    SensorReading(int type, int value, int source, float value_parsed);
    SensorReading(int type, int value, int source, float value_parsed, int boot);
    void post();
    void display();
    int _type;
    int _value;
    int _source;
    float _value_parsed;
    int _boot;
    int _id;
};
#endif
