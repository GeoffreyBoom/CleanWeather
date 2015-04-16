#include <pebble.h>
#include "weatherneat.h"
#include "window.h"
#include "weather.h"
static TextLayer *title_layer;
static TextLayer *time_layer;
static TextLayer *month_layer;
static TextLayer *temperature_layer;
static TextLayer *weekday_layer;
static TextLayer *condition_layer;
static TextLayer *year_layer;
static TextLayer *location_layer;
  
  
//=======================WEATHER=====================//

static AppSync s_sync;
static uint8_t s_sync_buffer[32];

static void request_weather(){
  //dummy code to ask for js to update
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }

  int value = 1;
  dict_write_int(iter, 1, &value, sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();
  
}
static void receive_weather(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context){
  switch(key){
    case WEATHER_CITY_KEY:
      set_text_location((char*)new_tuple->value->cstring);
      break;
    case WEATHER_TEMPERATURE_KEY:
      text_layer_set_text(temperature_layer, new_tuple->value->cstring);
      //set_text_temperature((char*)new_tuple->value->cstring);
      break;
    case WEATHER_CONDITION_KEY:
      set_text_condition((char*)new_tuple->value->cstring);
      break;
  }
}
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context){
  
}

static void init_weather(){
  app_message_open(64, 64);
  Tuplet initial_values[] = {
    TupletInteger(WEATHER_REQUEST_KEY, 1),
    TupletCString(WEATHER_CITY_KEY, "Montreal"),
    TupletCString(WEATHER_TEMPERATURE_KEY, "0°C"),
    TupletCString(WEATHER_CONDITION_KEY, "Windy")
  };
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, 
                ARRAY_LENGTH(initial_values), receive_weather, sync_error_handler, NULL);
  request_weather();
}
static void set_weather(struct Weather* weather){
  set_text_temperature(weather->temperature);
  set_text_condition(weather->condition);
  set_text_location(weather->location);
}

//=======================ENDWEATHER=====================//


//=======================INIT====================//

static void init(){
  show_window();
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  init_weather();
}
static void deinit(){
  hide_window();
  app_sync_deinit(&s_sync);
}
int main(void){
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

//=====================ENDINIT==================//

//============================WINDOW=======================//
  
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