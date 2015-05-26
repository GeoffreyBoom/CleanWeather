#include <pebble.h>
#define debug_mode false
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
  CONFIGURATION_LOCATION = 1,
  VERSION_LOCATION = 2
};

struct Weather{
  char location[32];
  char temperature[9];
  char condition[32];
  char forecast_1[15];
  char forecast_2[15];
  char forecast_3[15];
  char forecast_4[15];
  time_t time;
};
struct Configuration{
  int light_time;
  int weather_time;
  bool if_weather;
  bool if_forecast;
  bool if_callendar;
};
void set_version();

typedef struct Configuration Configuration;
typedef struct Weather Weather;
Configuration* get_configuration();
Weather* get_weather_buffer();

void set_light_time(int time);
int get_light_time();
void set_weather_time(int time);
int get_weather_time();