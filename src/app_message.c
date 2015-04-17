/*
 * main.c
 * Creates a Window and output TextLayer, then opens AppSync to 
 * keep up-to-date with a counter running in pebble-js-app.js (see below).
 */

#include <pebble.h>

enum weather_key{
  WEATHER_REQUEST_KEY = 0x0,
  WEATHER_CITY_KEY = 0x1,
  WEATHER_TEMPERATURE_KEY = 0x2,
  WEATHER_CONDITION_KEY = 0x3,
  WEATHER_TIME_STAMP = 0x4
};

enum storage_key{
  WEATHER_DATA_LOCATION = 0
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

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

static void init();
static void deinit();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed );
static void start_weather_timer();

void show_window(void);
void hide_window(void);

static void set_text_temperature(char*);
static void set_text_condition(char*);
static void set_text_location(char*);

static void set_text_year(char*);
static void set_text_month(char*);
static void set_text_weekday(char*);
static void set_text_time(char*);
  
void request_weather(void){

  DictionaryIterator *iter;
	
  app_message_outbox_begin(&iter);
  
  dict_write_end(iter);
  
  app_message_outbox_send();
  
}

static Window *s_main_window;;

static TextLayer *s_output_layer;
static AppSync s_sync;
static uint8_t s_sync_buffer[128];

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // Update the TextLayer output
  static char s_city_buffer[32];
  static char s_temperature_buffer[5];
  static char s_condition_buffer[32];
  switch(key){
    case WEATHER_CITY_KEY:
      snprintf(s_city_buffer, sizeof(s_city_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer.location, s_city_buffer, sizeof(s_city_buffer));
      set_text_location(s_city_buffer);
      break;
    case WEATHER_TEMPERATURE_KEY:
      snprintf(s_temperature_buffer, sizeof(s_temperature_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer.temperature, s_temperature_buffer, sizeof(s_temperature_buffer));
      set_text_temperature(s_temperature_buffer);
      break;
    case WEATHER_CONDITION_KEY:
      snprintf(s_condition_buffer, sizeof(s_condition_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer.condition, s_condition_buffer, sizeof(s_condition_buffer));
      set_text_condition(s_condition_buffer);
      break;
  }
  persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));

}

static void weather_callback(void* data){
  request_weather();
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
  switch (app_message_error) {
    case APP_MSG_OK:   APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_OK"); break;
    case APP_MSG_SEND_TIMEOUT: 
      APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_SEND_TIMEOUT");
      app_timer_register(1000, weather_callback, NULL);
      break;
    case APP_MSG_SEND_REJECTED: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_SEND_REJECTED");break;
    case APP_MSG_NOT_CONNECTED: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_NOT_CONNECTED");break;
    case APP_MSG_APP_NOT_RUNNING: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_APP_NOT_RUNNING");break;
    case APP_MSG_INVALID_ARGS: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_INVALID_ARGS");break;
    case APP_MSG_BUSY:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_BUSY");break;
    case APP_MSG_BUFFER_OVERFLOW:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_BUFFER_OVERFLOW"); break;
    case APP_MSG_ALREADY_RELEASED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_ALREADY_RELEASED"); break;
    case APP_MSG_CALLBACK_ALREADY_REGISTERED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_CALLBACK_ALREADY_REGISTERED");break;
    case APP_MSG_CALLBACK_NOT_REGISTERED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_CALLBACK_NOT_REGISTERED"); break;
    case APP_MSG_OUT_OF_MEMORY:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_OUT_OF_MEMORY");break;
    case APP_MSG_CLOSED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_CLOSED"); break;
    case APP_MSG_INTERNAL_ERROR:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_INTERNAL_ERROR");break;
    default:APP_LOG(APP_LOG_LEVEL_ERROR, "UNKNOWN ERROR"); break;
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output TextLayer
  s_output_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
  text_layer_set_text(s_output_layer, "Waiting...");
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
}

static void init(void) {
  show_window();
  
  // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  //load initial values
  //if value not in memory
  //create default initial values
  if(!(persist_exists(WEATHER_DATA_LOCATION)
     && persist_read_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather)) != E_DOES_NOT_EXIST)){
    strcpy(weather_buffer.location, "NULL");
    strcpy(weather_buffer.temperature, "NULL°C");
    strcpy(weather_buffer.condition, "NULL");
    weather_buffer.time = time(NULL);
    persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));
  }
  Tuplet initial_values[] = {
    TupletCString(WEATHER_CITY_KEY, weather_buffer.location),
    TupletCString(WEATHER_TEMPERATURE_KEY, weather_buffer.temperature),
    TupletCString(WEATHER_CONDITION_KEY, weather_buffer.condition)
  };
  
  // Begin Clock Ticking
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
  
  // Start Weather Updating
  start_weather_timer(NULL);
}

static void start_weather_timer(void* data){
  request_weather();
  app_timer_register(1000*60, start_weather_timer, NULL);
}

static void deinit(void) {
  // Destroy main Window
  hide_window();
  //window_destroy(s_main_window);

  // Finish using AppSync
  app_sync_deinit(&s_sync);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed ){
  time_t temp = (time(NULL));
  tick_time = localtime(&temp);
  static char dtime[]   = "00:00";
  static char month[]   = "---";
  static char weekday[] = "---";
  static char date[]    = "00";
  static char year[]    = "00";
  //set time
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(dtime,  sizeof("00:00"), "%R"    , tick_time);
  } else {
    // Use 12 hour format
    strftime(dtime,  sizeof("00:00"), "%I:%M:", tick_time);
  }
  
  //set month
  strftime(month,   sizeof("---"),   "%b"    , tick_time);
  
  //set weekday
  strftime(weekday, sizeof("---"),   "%a"    , tick_time);
  
  //set date
  strftime(date,    sizeof("00"),    "%d"    , tick_time);
  //set_text_date(date);
  
  //set year
  strftime(year,    sizeof("00"),    "%y"    , tick_time);
  set_text_year(year);
  set_text_month(month);
  set_text_weekday(weekday);
  set_text_time(dtime);
}
