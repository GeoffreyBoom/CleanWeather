#include <pebble.h>
#pragma once
enum sync_key{
  WEATHER_REQUEST_KEY = 0,
  WEATHER_CITY_KEY = 1,
  WEATHER_TEMPERATURE_KEY = 2,
  WEATHER_CONDITION_KEY = 3,
  WEATHER_TIME_STAMP = 4,
  LIGHT_TIME_KEY = 5,
  WEATHER_TIME_KEY = 6,
  FORECAST_KEY_1 = 7,
  FORECAST_KEY_2 = 8,
  FORECAST_KEY_3 = 9,
  FORECAST_KEY_4 = 10
};

enum storage_key{
  WEATHER_DATA_LOCATION = 0,
  CONFIGURATION_LOCATION = 1
};

struct Weather{
  char location[32];
  char temperature[9];
  char condition[32];
  char forecast_1[32];
  char forecast_2[32];
  char forecast_3[32];
  char forecast_4[32];
  time_t time;
};

struct Configuration{
  int light_time;
  int weather_time;
};
typedef struct Configuration Configuration;

typedef struct Weather Weather;

void set_light_time(int time);
int get_light_time();
void set_weather_time(int time);
int get_weather_time();

Configuration* get_configuration();


