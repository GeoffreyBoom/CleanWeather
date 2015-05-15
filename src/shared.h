#include <pebble.h>
#pragma once
enum weather_key{
  WEATHER_REQUEST_KEY = 0,
  WEATHER_CITY_KEY = 1,
  WEATHER_TEMPERATURE_KEY = 2,
  WEATHER_CONDITION_KEY = 3,
  WEATHER_TIME_STAMP = 4,
  LIGHT_TIME_KEY = 5,
  WEATHER_TIME_KEY = 6
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

struct Configuration{
  int light_time;
  int weather_time;
};

typedef struct Configuration Configuration;

typedef struct Weather Weather;
