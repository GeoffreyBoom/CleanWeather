#pragma once
#include "shared.h"
#include "window.h"



void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);
void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context);

void weather_neat_show_window(void);
void weather_neat_hide_window(void);

void weather_neat_handle_window_unload(Window* window);
void weather_neat_init();
void weather_neat_deinit();
void weather_neat_destroy_ui(void);
void weather_neat_tick_handler(struct tm *tick_time, TimeUnits units_changed );
void shake_handler(AccelAxisType axis, int32_t direction);
void weather_neat_initialise_ui();
void start_weather_timer(void* data);
void weather_callback(void* data);

void set_text_temperature(char*);
void set_text_condition(char*);
void set_text_location(char*);
void set_text_update_time(char*);

void set_text_title(char* title);

void setup_app_sync();

int* get_light_time();

Weather* get_weather_buffer();