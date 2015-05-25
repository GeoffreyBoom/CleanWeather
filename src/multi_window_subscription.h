#pragma once
#include "multi_window.h"
#include "array.h"
#include "stdio.h"
  
void multi_window_appsync_init(struct AppSync * s, uint8_t * buffer, const uint16_t buffer_size, 
                               const Tuplet *const keys_and_initial_values, const uint8_t count,
                               void * context);
void multi_window_app_sync_deinit(struct AppSync * s);

void multi_window_tick_timer_service_subscribe(TimeUnits tick_units, TickHandler tick_handler);
void multi_window_bluetooth_connection_service_subscribe(BluetoothConnectionHandler BT_handler);
void multi_window_battery_state_service_subscribe(BatteryStateHandler BS_handler);
void multi_window_accel_tap_service_subscribe(AccelTapHandler tap_handler);
void multi_window_app_sync_service_subscribe(AppSyncTupleChangedCallback change_handler);


void multi_window_tick_timer_service_unsubscribe(TickHandler tick_handler);
void multi_window_bluetooth_connection_service_unsubscribe(BluetoothConnectionHandler BT_handler);
void multi_window_battery_state_service_unsubscribe(BatteryStateHandler BS_handler);
void multi_window_accel_tap_service_unsubscribe(AccelTapHandler tap_handler);
void multi_window_app_sync_service_unsubscribe(AppSyncTupleChangedCallback change_handler);
