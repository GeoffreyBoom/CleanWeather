#pragma once
  
enum{
  WEATHER_CITY_KEY = 0x0,
  WEATHER_TEMPERATURE_KEY = 0x1,
  WEATHER_CONDITION_KEY = 0x2,
  WEATHER_TIME_STAMP = 0x3
};
struct Weather{
  char city[20];
  char temperature[5];
  char condition[20];
  int time;
};

static void set_weather(struct Weather*);
static void request_weather(void);
static void receive_weather(void);