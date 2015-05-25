#pragma once
#include <pebble.h>
#include "multi_window_subscription.h"
#include "sub_window.h"
#include "window_sequence.h"
#include "multi_window.h"
#include "array.h"
struct MultiWindow;

typedef struct MultiWindow MultiWindow;

struct MultiWindow{
  WindowSequence* window_sequence;
  TickHandler* tick_handlers;
  BluetoothConnectionHandler* bluetooth_connection_handlers;
  BatteryStateHandler* battery_state_handlers;
  AccelTapHandler* tap_handlers;
  AppSyncTupleChangedCallback* sync_handlers;
  TimeUnits time_units;
  int number_tick_handlers;
  int number_bluetooth_handlers;
  int number_battery_handlers;
  int number_tap_handlers;
  int number_sync_handlers;
  bool tick_in_use;
  bool bluetooth_in_use;
  bool battery_in_use;
};

MultiWindow* get_multi_window();

void multi_window_set_main_window(SubWindow*);
void multi_window_add_sub_window(SubWindow*);
//void multi_window_remove_sub_window(SubWindow*);
void multi_window_display_initial();
void multi_window_display_next();

void multi_window_shake_for_next(bool enabled);
