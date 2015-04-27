#include <pebble.h>
#pragma once
enum weather_key{
  WEATHER_REQUEST_KEY = 0x0,
  WEATHER_CITY_KEY = 0x1,
  WEATHER_TEMPERATURE_KEY = 0x2,
  WEATHER_CONDITION_KEY = 0x3,
  WEATHER_TIME_STAMP = 0x4,
  LIGHT_TIME_KEY = 0x5
};

enum storage_key{
  WEATHER_DATA_LOCATION = 0,
  CONFIGURATION_LOCATION = 1
};

struct Weather{
  char location[32];
  char temperature[9];
  char condition[32];
  time_t time;
};
