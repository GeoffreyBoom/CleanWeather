#pragma once
  
enum{
  WEATHER_REQUEST_KEY = 0x0,
  WEATHER_CITY_KEY = 0x1,
  WEATHER_TEMPERATURE_KEY = 0x2,
  WEATHER_CONDITION_KEY = 0x3,
  WEATHER_TIME_STAMP = 0x4
};
struct Weather{
  char location[20];
  char temperature[5];
  char condition[20];
  int time;
};

static void set_weather(struct Weather*);
static void request_weather(void);
static void receive_weather(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);
