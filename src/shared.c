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
    if((persist_read_data(CONFIGURATION_LOCATION, config, sizeof(config)) == E_DOES_NOT_EXIST)){
      config->light_time = 6;
      config->weather_time = 10;
    }
  }
  return config;
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