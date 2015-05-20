#pragma once
#include "weather_neat.h"
#include "shared.h"
void show_forecast_window(void);
void hide_forecast_window(void);
void set_text_forecast_1(char*);
void set_text_forecast_2(char*);
void set_text_forecast_3(char*);
void set_text_forecast_4(char*);

Layer* get_forecast_layer();