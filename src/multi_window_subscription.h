#pragma once
#include "sub_window.h"
void multi_window_tick_service_subscribe(TimeUnits tick_units, TickHandler tick_handler);
void multi_window_bluetooth_connection_service_subscribe(BluetoothConnectionHandler BT_handler);
void multi_window_battery_state_service_subscribe(BatteryStateHandler BS_handler);