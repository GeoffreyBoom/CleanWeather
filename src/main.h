#include <pebble.h>
#pragma once

static Window *s_window;
static TextLayer *title_layer;
static TextLayer *time_layer;
static TextLayer *month_layer;
static TextLayer *temperature_layer;
static TextLayer *weekday_layer;
static TextLayer *condition_layer;
static TextLayer *year_layer;
static TextLayer *location_layer;
static TextLayer *update_time_layer;

static BitmapLayer *bluetooth_layer = NULL;
static GBitmap *bluetooth_icon = NULL;
static BitmapLayer *battery_layer = NULL;
static GBitmap *battery_icon = NULL;
static BitmapLayer *battery_charging_layer = NULL;
static GBitmap *battery_charging_icon = NULL;
static int battery_level = 10;



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
static struct Weather weather_buffer;

void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

static void init();
static void deinit();
void tick_handler(struct tm *tick_time, TimeUnits units_changed );
static void bluetooth_handler(bool bluetooth);
static void battery_handler(BatteryChargeState battery);
static void shake_handler(AccelAxisType axis, int32_t direction);
static void start_weather_timer();

static int light_time = 10;

void show_window(void);
void hide_window(void);

void set_text_temperature(char*);
void set_text_condition(char*);
void set_text_location(char*);
static void set_text_update_time(char*);

static void set_text_year(char*);
static void set_text_month(char*);
static void set_text_weekday(char*);
static void set_text_time(char*);

static void set_text_title(char* title);

static void setup_app_sync();

static AppSync s_sync;
static uint8_t s_sync_buffer[128];