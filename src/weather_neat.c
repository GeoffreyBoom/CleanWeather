#include <pebble.h>
#include "weather_neat.h"
#include "multi_window_subscription.h"

TextLayer *temperature_layer;
TextLayer *location_layer;
TextLayer *update_time_layer;
TextLayer *condition_layer;

void weather_neat_populate_ui();

void weather_neat_init(void) {
  weather_neat_initialise_ui();
  weather_neat_populate_ui();
  // begin DT counting
  multi_window_tick_timer_service_subscribe(MINUTE_UNIT, weather_neat_tick_handler);
  weather_neat_tick_handler(NULL, MINUTE_UNIT);
  //register shake handler
  multi_window_accel_tap_service_subscribe(shake_handler);
  
}

void weather_neat_deinit(void) {
  
  weather_neat_destroy_ui();
  
  multi_window_tick_timer_service_unsubscribe(weather_neat_tick_handler);
  
  multi_window_accel_tap_service_unsubscribe(shake_handler);
  
}

void light_off(void* data){
  set_text_title("CleanWeather");
  light_enable(false);
}


void shake_handler(AccelAxisType axis, int32_t direction){
  int light_time = get_light_time();
  light_enable(true);
  app_timer_register(1000*light_time, light_off, NULL);
  set_text_title("Light!");
}


Weather* get_weather_buffer(){
  static Weather* weather_buffer = NULL;
  if(!weather_buffer){
    weather_buffer = malloc(sizeof(Weather));
    if(!(persist_exists(WEATHER_DATA_LOCATION)
       && persist_read_data(WEATHER_DATA_LOCATION, weather_buffer, sizeof(struct Weather)) != E_DOES_NOT_EXIST)){
      strcpy(weather_buffer->location, "NULL");
      strcpy(weather_buffer->temperature, "NULL°C");
      strcpy(weather_buffer->condition, "NULL");
      strcpy(weather_buffer->forecast_1, "00:00 00°C");
      strcpy(weather_buffer->forecast_2, "00:00 00°C");
      strcpy(weather_buffer->forecast_3, "00:00 00°C");
      weather_buffer->time = 100;
      persist_write_data(WEATHER_DATA_LOCATION, weather_buffer, sizeof(struct Weather));
    }
  }
  return weather_buffer;
}

Configuration* get_configuration(){
  static Configuration* config = NULL;
  if(!config){
    config = malloc(sizeof(Configuration));
    if((persist_read_data(CONFIGURATION_LOCATION, config, sizeof(config)) == E_DOES_NOT_EXIST)){
      config->light_time = 6;
      config->weather_time = 10;
    }
  }
  return config;
}

int get_light_time(){
  Configuration* config = get_configuration();
  return config->light_time;
}

void set_light_time(int time){
  Configuration* config = get_configuration();
  config->light_time = time;
  persist_write_data(CONFIGURATION_LOCATION, config, sizeof(Configuration));
}

int get_weather_time(){
  Configuration* config = get_configuration();
  return config->weather_time;
}

void set_weather_time(int time){
  Configuration* config = get_configuration();
  config->weather_time = time;
  persist_write_data(CONFIGURATION_LOCATION, config, sizeof(Configuration));
}

void weather_neat_tick_handler(struct tm *tick_time, TimeUnits units_changed ){
  time_t temp = (time(NULL));
  Weather weather_buffer = *get_weather_buffer();

  static char sdeltat[10];
  long deltat = (weather_buffer.time - temp)/60;
  if(deltat > 10){
    snprintf(sdeltat, sizeof(sdeltat), "DT:!%li!", deltat);
  }
  else{
    snprintf(sdeltat, sizeof(sdeltat), "DT:%li", deltat);
  }
  set_text_update_time(sdeltat);
}

static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static GFont s_res_bitham_30_black;

void weather_neat_initialise_ui(void) {
  Window* s_window = get_window();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  
  // temperature_layer
  temperature_layer = text_layer_create(GRect(88, 90, 52, 39));
  text_layer_set_text(temperature_layer, "15°C");
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  text_layer_set_font(temperature_layer, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)temperature_layer);
  
  // condition_layer
  condition_layer = text_layer_create(GRect(4, 90, 80, 39));
  text_layer_set_text(condition_layer, "Scattered Clouds");
  text_layer_set_text_alignment(condition_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)condition_layer);
  
  // location_layer
  location_layer = text_layer_create(GRect(4, 144, 94, 20));
  text_layer_set_text(location_layer, "Hampsted");
  text_layer_set_text_alignment(location_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)location_layer);
  
  // update_time_layer
  update_time_layer = text_layer_create(GRect(100, 144, 40, 20));
  text_layer_set_text(update_time_layer, "DT:10");
  text_layer_set_text_alignment(update_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)update_time_layer);
}

void weather_neat_populate_ui(){
  Weather* weather_buffer = get_weather_buffer();
  set_text_temperature(weather_buffer->temperature);
  set_text_condition(weather_buffer->condition);
  set_text_location(weather_buffer->location);
}

void weather_neat_destroy_ui(void) {
  layer_remove_from_parent(text_layer_get_layer(condition_layer));
  layer_remove_from_parent(text_layer_get_layer(location_layer));
  layer_remove_from_parent(text_layer_get_layer(update_time_layer));
  layer_remove_from_parent(text_layer_get_layer(temperature_layer));
  text_layer_destroy(temperature_layer);
  text_layer_destroy(condition_layer);
  text_layer_destroy(location_layer);
  text_layer_destroy(update_time_layer);
  temperature_layer = NULL;
  condition_layer = NULL;
  location_layer = NULL;
  update_time_layer = NULL;
}

//weather functions
void set_text_temperature(char* temperature){
  if(temperature_layer){
    text_layer_set_text(temperature_layer, temperature);
  }
}
void set_text_condition(char* condition){
  if(condition_layer){
    text_layer_set_text(condition_layer, condition);
  }
}
void set_text_location(char* location){
  if(location_layer){
    text_layer_set_text(location_layer, location);
  }
}
void set_text_update_time(char* update_time){
  if(update_time_layer){
    text_layer_set_text(update_time_layer, update_time);
  }
}