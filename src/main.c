#include "main.h"
#include <stdio.h>
//EXAMPLE CODE//

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

void request_weather(void* data){
  
  printf("dictionary\n");

  DictionaryIterator *iter;
  
  printf("app_message\n");
	
  app_message_outbox_begin(&iter);
  
  printf("dict_write\n");
  
  dict_write_end(iter);
  
  printf("app_message_send\n");
  
  app_message_outbox_send();
  
}

void setup_app_sync(){
  // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  Weather weather_buffer = *get_weather_buffer();
  int light_time = get_light_time();
  int weather_time = get_weather_time();
  
  Tuplet initial_values[] = {
    TupletInteger(WEATHER_REQUEST_KEY, 0),
    TupletCString(WEATHER_CITY_KEY, weather_buffer.location),
    TupletCString(WEATHER_TEMPERATURE_KEY, weather_buffer.temperature),
    TupletCString(WEATHER_CONDITION_KEY, weather_buffer.condition),
    TupletInteger(LIGHT_TIME_KEY, light_time),
    TupletInteger(WEATHER_TIME_KEY, weather_time),
    TupletCString(FORECAST_KEY_1, weather_buffer.forecast_1),
    TupletCString(FORECAST_KEY_2, weather_buffer.forecast_2),
    TupletCString(FORECAST_KEY_3, weather_buffer.forecast_3),
    TupletCString(FORECAST_KEY_4, weather_buffer.forecast_4)
  };
  // Begin using AppSync
  multi_window_appsync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), NULL);
}

void start_weather_timer(void* data){
  request_weather(NULL);
  static AppTimer* timer = NULL;
  if(timer){
    app_timer_cancel(timer);
  }
  int weather_time = get_weather_time();
  if(weather_time != 0){
    printf("weather time was not 0");
    timer = app_timer_register(1000*60*10, start_weather_timer, NULL);
  }
  else{
    printf("weather time was 0");
    timer = app_timer_register(1000*60*10, start_weather_timer, NULL);
  }
}

void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // Update the TextLayer output
  static char s_city_buffer[32];
  static char s_temperature_buffer[5];
  static char s_condition_buffer[32];
  Weather* weather_buffer = get_weather_buffer();
  switch(key){
    case WEATHER_CITY_KEY:
      snprintf(s_city_buffer, sizeof(s_city_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->location, s_city_buffer, sizeof(s_city_buffer));
      set_text_location(s_city_buffer);
      break;
    case WEATHER_TEMPERATURE_KEY:
      snprintf(s_temperature_buffer, sizeof(s_temperature_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->temperature, s_temperature_buffer, sizeof(s_temperature_buffer));
      set_text_temperature(s_temperature_buffer);
      break;
    case WEATHER_CONDITION_KEY:
      snprintf(s_condition_buffer, sizeof(s_condition_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->condition, s_condition_buffer, sizeof(s_condition_buffer));
      set_text_condition(s_condition_buffer);
      break;
    case WEATHER_REQUEST_KEY:
      printf("%i",(int)new_tuple->value->int32);
      if((int)new_tuple->value->int32){
        weather_buffer->time = time(NULL);
      }
      break;
  }
  persist_write_data(WEATHER_DATA_LOCATION, weather_buffer, sizeof(struct Weather));
  weather_neat_tick_handler(NULL, MINUTE_UNIT);
}

void forecast_sync_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tupple, void *context){
  static char for_1[32];
  static char for_2[32];
  static char for_3[32];
  static char for_4[32];
    Weather* weather_buffer = get_weather_buffer();
  switch(key){
    case FORECAST_KEY_1:
      snprintf(for_1, sizeof(for_1), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->forecast_1, for_1, sizeof(for_1));
      set_text_forecast_1(weather_buffer->forecast_1);
      break;
    case FORECAST_KEY_2:
      snprintf(for_2, sizeof(for_2), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->forecast_2, for_2, sizeof(for_2));
      set_text_forecast_2(weather_buffer->forecast_2);
      break;
    case FORECAST_KEY_3:
      snprintf(for_3, sizeof(for_3), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->forecast_3, for_3, sizeof(for_3));
      set_text_forecast_3(weather_buffer->forecast_3);
      printf("%s", weather_buffer->forecast_3);
      break;
    case FORECAST_KEY_4:
      snprintf(for_4, sizeof(for_4), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer->forecast_4, for_4, sizeof(for_4));
      set_text_forecast_4(weather_buffer->forecast_4);
      printf("forecast 4:%s", weather_buffer->forecast_4);
      break;
  }
  persist_write_data(WEATHER_DATA_LOCATION, weather_buffer, sizeof(struct Weather));
}

void configuration_sync_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context){
  printf("starting configuration sync handler\n");
  switch(key){
    //printf("key: %i", (int)key);
    case LIGHT_TIME_KEY:
      set_light_time((int)new_tuple->value->int32);
      break;
    case WEATHER_TIME_KEY:
      set_weather_time((int)new_tuple->value->int32);
      printf("%i", get_weather_time());
      break;
  }
}

void weather_callback(void* data){
  request_weather(NULL);
}
  
void main_init(void* nothing, int none){
  init();
}
void main_de_init(void* nothing, int none){
  deinit();
}

void weather_init(void* nothing, int none){
  weather_neat_init();
}
void weather_deinit(void* nothing, int none){
  weather_neat_deinit();
}

void test_init(void* nothing, int none){
  show_forecast_window();
}
void test_deinit(void* nothing, int none){
  hide_forecast_window();
}

int main(void){
  //subscribe to appsync handler
  multi_window_app_sync_service_subscribe(sync_changed_handler);
  multi_window_app_sync_service_subscribe(forecast_sync_handler);
  multi_window_app_sync_service_subscribe(configuration_sync_handler);
  
  // Begin App Sync
  setup_app_sync();  
  // Start Weather Updating
  start_weather_timer(NULL);
  
  multi_window_set_main_window(sub_window_create(main_init, main_de_init,NULL, NULL, NULL));
  multi_window_add_sub_window(sub_window_create(weather_init, weather_deinit, NULL, NULL, NULL));
  multi_window_add_sub_window(sub_window_create(test_init, test_deinit, NULL, NULL, get_forecast_layer));

  multi_window_display_initial();
  multi_window_shake_for_next(true);
  /*
  */
  printf("free: %i", (int)heap_bytes_free());
  printf("used: %i", (int)heap_bytes_used());
  app_event_loop();
  
  // Finish using AppSync
  multi_window_app_sync_deinit(&s_sync);
  multi_window_app_sync_service_unsubscribe(sync_changed_handler);
  //sub_window_de_display(sequence->sub_window_array[sequence->current_window]);
}