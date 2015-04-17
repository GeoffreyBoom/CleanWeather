/*
 * main.c
 * Creates a Window and output TextLayer, then opens AppSync to 
 * keep up-to-date with a counter running in pebble-js-app.js (see below).
 */

#include <pebble.h>

static TextLayer *title_layer;
static TextLayer *time_layer;
static TextLayer *month_layer;
static TextLayer *temperature_layer;
static TextLayer *weekday_layer;
static TextLayer *condition_layer;
static TextLayer *year_layer;
static TextLayer *location_layer;
  
#define KEY_COUNT 5
enum weather_key{
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

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

static void init();
static void deinit();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed );

void show_window(void);
void hide_window(void);

static void set_text_temperature(char*);
static void set_text_condition(char*);
static void set_text_location(char*);

static void set_text_year(char*);
static void set_text_month(char*);
static void set_text_weekday(char*);
static void set_text_time(char*);

//their code//
  
void send_message(void){

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
  static char s_count_buffer[32];
  static char s_city_buffer[32];
  static char s_temperature_buffer[32];
  static char s_condition_buffer[32];
  switch(key){
    //case KEY_COUNT:
      //snprintf(s_count_buffer, sizeof(s_count_buffer), "count: %d", (int)new_tuple->value->int32);
      //text_layer_set_text(s_output_layer, s_count_buffer);
      //set_text_condition(s_count_buffer);
      //break;
    case WEATHER_CITY_KEY:
      snprintf(s_city_buffer, sizeof(s_city_buffer), "%s", (char*)new_tuple->value->cstring);
      set_text_location(s_city_buffer);
      break;
    case WEATHER_TEMPERATURE_KEY:
      snprintf(s_temperature_buffer, sizeof(s_temperature_buffer), "%s", (char*)new_tuple->value->cstring);
      set_text_temperature(s_temperature_buffer);
      break;
    case WEATHER_CONDITION_KEY:
      snprintf(s_condition_buffer, sizeof(s_condition_buffer), "%s", (char*)new_tuple->value->cstring);
      set_text_condition(s_condition_buffer);
      break;
  }
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
  switch (app_message_error) {
    case APP_MSG_OK:   APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_OK"); break;
    case APP_MSG_SEND_TIMEOUT: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_SEND_TIMEOUT");break;
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
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  show_window();
  // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Setup initial values
  Tuplet initial_values[] = {
    TupletInteger(KEY_COUNT, 0),
    TupletCString(WEATHER_CITY_KEY, "Montreal"),
    TupletCString(WEATHER_TEMPERATURE_KEY, "0°C"),
    TupletCString(WEATHER_CONDITION_KEY, "Windy")
  };

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  send_message();
}

static void deinit(void) {
  // Destroy main Window
  hide_window();
  window_destroy(s_main_window);

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

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_18_bold;


static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  // title_layer
  title_layer = text_layer_create(GRect(0, 0, 144, 16));
  text_layer_set_text(title_layer, "CleanWeather");
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)title_layer);
  
  // time_layer
  time_layer = text_layer_create(GRect(4, 80, 80, 38));
  text_layer_set_text(time_layer, "NULL");
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)time_layer);
  
  // month_layer
  month_layer = text_layer_create(GRect(99, 20, 42, 30));
  text_layer_set_background_color(month_layer, GColorClear);
  text_layer_set_text_color(month_layer, GColorWhite);
  text_layer_set_text(month_layer, "JUL");
  text_layer_set_text_alignment(month_layer, GTextAlignmentRight);
  text_layer_set_font(month_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)month_layer);
  
  // temperature_layer
  temperature_layer = text_layer_create(GRect(87, 80, 52, 38));
  text_layer_set_text(temperature_layer, "LOAD");
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  text_layer_set_font(temperature_layer, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)temperature_layer);
  
  // weekday_layer
  weekday_layer = text_layer_create(GRect(50, 20, 42, 28));
  text_layer_set_background_color(weekday_layer, GColorClear);
  text_layer_set_text_color(weekday_layer, GColorWhite);
  text_layer_set_text(weekday_layer, "WED");
  text_layer_set_text_alignment(weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(weekday_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)weekday_layer);
  
  // condition_layer
  condition_layer = text_layer_create(GRect(4, 55, 134, 18));
  text_layer_set_background_color(condition_layer, GColorClear);
  text_layer_set_text_color(condition_layer, GColorWhite);
  text_layer_set_text(condition_layer, "LOADING");
  text_layer_set_text_alignment(condition_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)condition_layer);
  
  // year_layer
  year_layer = text_layer_create(GRect(4, 20, 42, 28));
  text_layer_set_background_color(year_layer, GColorClear);
  text_layer_set_text_color(year_layer, GColorWhite);
  text_layer_set_text(year_layer, "92");
  text_layer_set_font(year_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)year_layer);
  
  // location_layer
  location_layer = text_layer_create(GRect(5, 130, 134, 24));
  text_layer_set_text(location_layer, "LOADING");
  text_layer_set_text_alignment(location_layer, GTextAlignmentCenter);
  text_layer_set_font(location_layer, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)location_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(title_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(month_layer);
  text_layer_destroy(temperature_layer);
  text_layer_destroy(weekday_layer);
  text_layer_destroy(condition_layer);
  text_layer_destroy(year_layer);
  text_layer_destroy(location_layer);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

//time functions
static void set_text_year(char* year){
  text_layer_set_text(year_layer, year);
}
static void set_text_month(char* month){
  text_layer_set_text(month_layer, month);
}
static void set_text_weekday(char* weekday){
  text_layer_set_text(weekday_layer, weekday);
}
static void set_text_time(char* time){
  text_layer_set_text(time_layer, time);
}

//weather functions
static void set_text_temperature(char* temperature){
  text_layer_set_text(temperature_layer, temperature);
}
static void set_text_condition(char* condition){
  text_layer_set_text(condition_layer, condition);
}
static void set_text_location(char* location){
  text_layer_set_text(location_layer, location);
}

void show_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_window(void) {
  window_stack_remove(s_window, true);
}

//=======================ENDWINDOW=====================//