#pragma once
#include "shared.h"
#include "window.h"

void weather_neat_show_window(void);
void weather_neat_hide_window(void);

void weather_neat_handle_window_unload(Window* window);
void weather_neat_init();
void weather_neat_deinit();
void weather_neat_destroy_ui(void);
void weather_neat_tick_handler(struct tm *tick_time, TimeUnits units_changed );
void weather_neat_initialise_ui();
void start_weather_timer(void* data);
void weather_callback(void* data);

void set_text_temperature(char*);
void set_text_condition(char*);
void set_text_location(char*);
void set_text_update_time(char*);

void set_text_title(char* title);

Weather* get_weather_buffer();