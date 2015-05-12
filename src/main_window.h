#include <pebble.h>
#pragma once
#include "shared.h"
#include "window.h"
#include "multi_window_subscription.h"


//a static buffer for loading and storing in memory
//the weather on the persist_storage

void init();
void deinit();
void tick_handler(struct tm *tick_time, TimeUnits units_changed );
void bluetooth_handler(bool bluetooth);
void battery_handler(BatteryChargeState battery);

void show_window(void);
void hide_window(void);

void set_text_year(char*);
void set_text_month(char*);
void set_text_weekday(char*);
void set_text_time(char*);

void set_text_title(char* title);

