#include <pebble.h>
#include "weather_neat.h"
#include "multi_window_subscription.h"

AppSync s_sync;
uint8_t s_sync_buffer[256];
struct Weather weather_buffer;

TextLayer *temperature_layer;
TextLayer *location_layer;
TextLayer *update_time_layer;
TextLayer *condition_layer;


int light_time = 10;

void weather_neat_init(void) {
  
  weather_neat_initialise_ui();
  
  // Begin App Sync
  setup_app_sync();
  
  // begin DT counting
  multi_window_tick_timer_service_subscribe(MINUTE_UNIT, weather_neat_tick_handler);
  
  //register shake handler
  accel_tap_service_subscribe(shake_handler);

  // Start Weather Updating
  start_weather_timer(NULL);
  
}

void weather_neat_deinit(void) {
  weather_neat_destroy_ui();
  
  // Finish using AppSync
  app_sync_deinit(&s_sync);
}


void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
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

void request_weather(void){

  DictionaryIterator *iter;
	
  app_message_outbox_begin(&iter);
  
  dict_write_end(iter);
  
  app_message_outbox_send();
  
}

void weather_callback(void* data){
  request_weather();
}

void light_off(void* data){
  set_text_title("CleanWeather");
  light_enable(false);
}


void shake_handler(AccelAxisType axis, int32_t direction){
  light_enable(true);
  app_timer_register(1000*light_time, light_off, NULL);
  set_text_title("Light!");
}



void setup_app_sync(){
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
    weather_buffer.time = 100;
    persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));
  }
  if(!(persist_read_data(CONFIGURATION_LOCATION, &light_time, sizeof(light_time)) != E_DOES_NOT_EXIST)){
    light_time=6;
  }
  Tuplet initial_values[] = {
    TupletInteger(WEATHER_REQUEST_KEY, 0),
    TupletCString(WEATHER_CITY_KEY, weather_buffer.location),
    TupletCString(WEATHER_TEMPERATURE_KEY, weather_buffer.temperature),
    TupletCString(WEATHER_CONDITION_KEY, weather_buffer.condition),
    TupletInteger(LIGHT_TIME_KEY, light_time)
  };
  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

void start_weather_timer(void* data){
  request_weather();
  app_timer_register(1000*60*10, start_weather_timer, NULL);
}

void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
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
    case LIGHT_TIME_KEY:
      light_time= (int)new_tuple->value->int32;
      break;
    case WEATHER_REQUEST_KEY:
      printf("%i",(int)new_tuple->value->int32);
      if((int)new_tuple->value->int32){
        printf("shouldnt be 0: %i",(int)new_tuple->value->int32);
        weather_buffer.time = time(NULL);
      }
      break;
  }
  persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));
  persist_write_data(CONFIGURATION_LOCATION, &light_time, sizeof(light_time));
  weather_neat_tick_handler(NULL, MINUTE_UNIT);
}


void weather_neat_tick_handler(struct tm *tick_time, TimeUnits units_changed ){
  time_t temp = (time(NULL));
  //tick_time = localtime(&temp);

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

void weather_neat_destroy_ui(void) {
  layer_remove_from_parent(text_layer_get_layer(condition_layer));
  layer_remove_from_parent(text_layer_get_layer(location_layer));
  layer_remove_from_parent(text_layer_get_layer(update_time_layer));
  layer_remove_from_parent(text_layer_get_layer(temperature_layer));
  text_layer_destroy(temperature_layer);
  text_layer_destroy(condition_layer);
  text_layer_destroy(location_layer);
  text_layer_destroy(update_time_layer);
}

//weather functions
void set_text_temperature(char* temperature){
  text_layer_set_text(temperature_layer, temperature);
}
void set_text_condition(char* condition){
  text_layer_set_text(condition_layer, condition);
}
void set_text_location(char* location){
  text_layer_set_text(location_layer, location);
}
void set_text_update_time(char* update_time){
  text_layer_set_text(update_time_layer, update_time);
}