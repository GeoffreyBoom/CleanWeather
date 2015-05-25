#include <pebble.h>
#include "shared.h"

int get_light_time(){
  Configuration* config = get_configuration();
  return config->light_time;
}

void set_light_time(int time){
  Configuration* config = get_configuration();
  config->light_time = time;
  persist_write_data(CONFIGURATION_LOCATION, config, sizeof(Configuration));
}

Configuration* get_configuration(){
  static Configuration* config = NULL;
  if(!config){
    config = malloc(sizeof(Configuration));
    if((persist_read_data(CONFIGURATION_LOCATION, config, sizeof(Configuration)) == E_DOES_NOT_EXIST)){
      config->light_time = 6;
      config->weather_time = 10;
      config->if_forecast = true;
      config->if_weather = true;
      config->if_callendar = true;
    }
  }
  return config;
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


int get_weather_time(){
  Configuration* config = get_configuration();
  return config->weather_time;
}

void set_weather_time(int time){
  Configuration* config = get_configuration();
  config->weather_time = time;
  persist_write_data(CONFIGURATION_LOCATION, config, sizeof(Configuration));
}

void set_version(){
  int version = 0;
  persist_write_data(VERSION_LOCATION, &version, sizeof(int));
}


