#include "main.h"

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
  }
  weather_buffer.time = time(NULL);
  persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));
  persist_write_data(CONFIGURATION_LOCATION, &light_time, sizeof(light_time));
  tick_handler(NULL, MINUTE_UNIT);
}
