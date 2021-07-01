String get_sensor_type(int type) { 
  switch(type) {
    case READING_TYPE_VOLTAGE:
      return "voltage";
      break;
    case READING_TYPE_SOIL:
      return "soil";
      break;
    case READING_TYPE_TEMPERATURE:
      return "temperature";
      break;
    case READING_TYPE_HUMIDITY:
      return "humidity";
      break;
    case READING_TYPE_RAIN:
      return "rain";
      break;
    case READING_TYPE_PRESSURE:
      return "pressure";
      break;
    case READING_TYPE_FLOAT:
      return "float";
      break;
    case READING_TYPE_PH:
      return "ph";
      break;
    case READING_TYPE_EC:
      return "ec";
      break;
    case READING_TYPE_LIGHT:
      return "light";
      break;
    case READING_TYPE_HEATINDEX:
      return "heat_index";
      break;
    default:
      return "sensor";
      break;
  }
}
