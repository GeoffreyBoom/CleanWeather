#include <pebble.h>
#pragma once
#include "shared.h"
#include "window.h"
#include "multi_window_subscription.h"


//a static buffer for loading and storing in memory
//the weather on the persist_storage

void init();
void deinit();

void set_text_year(char*);
void set_text_month(char*);
void set_text_weekday(char*);
void set_text_time(char*);

void set_text_title(char* title);

void main_window_message(char* message);