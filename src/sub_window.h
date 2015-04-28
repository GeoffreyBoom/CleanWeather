#pragma once
#include <pebble.h>
#include "main.h"
#include "weather_neat.h"

struct MultiWindow;

struct WindowSequence;
  
struct SubWindow;

typedef struct MultiWindow MultiWindow;

typedef struct SubWindow SubWindow;

typedef struct WindowSequence WindowSequence;

void window_sequence_add_sub_window(SubWindow* sub_window, WindowSequence* sequence);

void window_sequence_display_next(WindowSequence* sequence);

MultiWindow* get_multi_window();

struct MultiWindow{
  WindowSequence* window_sequence;
  TickHandler* tick_handlers;
  BluetoothConnectionHandler* bluetooth_connection_handlers;
  BatteryStateHandler* battery_state_handlers;
  AccelTapHandler* tap_handlers;
  TimeUnits time_units;
  int number_tick_handlers;
  int number_bluetooth_handlers;
  int number_battery_handlers;
  int number_tap_handlers;
  bool tick_in_use;
  bool bluetooth_in_use;
  bool battery_in_use;
};

struct WindowSequence{
  SubWindow* main_window;
  SubWindow** sub_window_array;
  int num_sub_windows;
  int current_window;
};
  
struct SubWindow{
  void (* init)(void* args, int num_args);
  void (* de_init)(void* args, int num_args);
  void (* service_subscribe)(void* args, int num_args);
  void (* service_unsubscribe) (void* args, int num_args);
  bool displayed;
};
