#pragma once
#include <pebble.h>
#include "main.h"
#include "weather_neat.h"

struct WindowSequence;
  
struct SubWindow;

typedef struct SubWindow SubWindow;
typedef struct WindowSequence WindowSequence;

void window_sequence_add_sub_window(SubWindow* sub_window, WindowSequence* sequence);

void window_sequence_display_next(WindowSequence* sequence);