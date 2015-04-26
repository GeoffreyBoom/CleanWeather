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

//a static buffer for loading and storing in memory
//the weather on the persist_storage

void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

void init();
void deinit();
void tick_handler(struct tm *tick_time, TimeUnits units_changed );
void bluetooth_handler(bool bluetooth);
void battery_handler(BatteryChargeState battery);
void shake_handler(AccelAxisType axis, int32_t direction);
void start_weather_timer();

void show_window(void);
void hide_window(void);

void set_text_temperature(char*);
void set_text_condition(char*);
void set_text_location(char*);
void set_text_update_time(char*);

void set_text_year(char*);
void set_text_month(char*);
void set_text_weekday(char*);
void set_text_time(char*);

void set_text_title(char* title);

void setup_app_sync();
