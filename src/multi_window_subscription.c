#include <pebble.h>
#include <stdio.h>
#include "multi_window_subscription.h"
#include "stdio.h"
enum handler_types{
  TICK = 0,
  BLUETOOTH=1,
  BATTERY=2,
  TAP=3
};

typedef void (*Function)();
void add_handler(Function** pointer_to_handlers, Function new_handler, int number_handlers, int type);

struct TickSubscription{
  TimeUnits tick_units;
  TickHandler tick_handler;
};typedef struct TickSubscription TickSubscription;

void tick_service_subscribe_callback(void* tick_sub){
  TickSubscription* ts = (TickSubscription*) tick_sub;
  TickSubscription tick_subscription;
  tick_subscription.tick_units = ts->tick_units;
  tick_subscription.tick_handler = ts->tick_handler;
  free(ts);
  multi_window_tick_timer_service_subscribe(tick_subscription.tick_units, tick_subscription.tick_handler);
}

void multi_window_tick_handler(struct tm *tick_time, TimeUnits units_changed){
  MultiWindow* multi_window = get_multi_window();
  for(int i = 0; i < multi_window->number_tick_handlers; i++){
    multi_window->tick_handlers[i](tick_time,units_changed);
  }
}
void multi_window_bluetooth_handler(bool bluetooth){
  MultiWindow* multi_window = get_multi_window();
  for(int i = 0; i < multi_window->number_bluetooth_handlers; i++){
    multi_window->bluetooth_connection_handlers[i](bluetooth);
  }
}

void multi_window_battery_handler(BatteryChargeState charge){
  MultiWindow* multi_window = get_multi_window();
  for(int i = 0; i < multi_window->number_battery_handlers; i++){
    multi_window->battery_state_handlers[i](charge);
  }
}

void multi_window_tap_handler(AccelAxisType axis, int32_t direction){
  MultiWindow* multi_window = get_multi_window();
  for(int i = 0; i < multi_window->number_tap_handlers; i++){
    multi_window->tap_handlers[i](axis, direction);
  }
}

void multi_window_tick_timer_service_subscribe(TimeUnits tick_units, TickHandler tick_handler){
  MultiWindow* multi_window = get_multi_window();
  //assuring that the tick service subscription won't be disturbed
  if(multi_window->tick_in_use){
    TickSubscription* ts = NULL;
    
    ts = malloc(sizeof(TickSubscription));
    ts->tick_units = tick_units;
    ts->tick_handler = tick_handler;
    
    app_timer_register(890,tick_service_subscribe_callback,(void*)ts);
  }
  else{
    multi_window->tick_in_use = true;
    
    printf("setting up a time listener\n");
    multi_window->number_tick_handlers+=1;
    add_handler((Function**)&(multi_window->tick_handlers), (Function) tick_handler, multi_window->number_tick_handlers, TICK);
    
    multi_window->tick_in_use = false;
  }
}

//todo: implement synchronization for the other types of handlers.
void multi_window_bluetooth_connection_service_subscribe(BluetoothConnectionHandler BT_handler){
  MultiWindow* multi_window = get_multi_window();
  multi_window->number_bluetooth_handlers+=1;
  add_handler((Function**)&(multi_window->bluetooth_connection_handlers), (Function) BT_handler, multi_window->number_bluetooth_handlers, BLUETOOTH);
}

void multi_window_battery_state_service_subscribe(BatteryStateHandler BS_handler){
  MultiWindow* multi_window = get_multi_window();
  multi_window->number_battery_handlers+=1;
  add_handler(&(multi_window->battery_state_handlers), BS_handler, multi_window->number_battery_handlers, BATTERY);
}


void add_handler(Function** pointer_to_handlers, Function new_handler, int number_handlers, int type){
  printf("adding handler\n");
  Function* old_handlers = *pointer_to_handlers;
  Function* new_handlers = malloc(sizeof(Function)*(number_handlers));
  for(int i = 0; i < number_handlers-1; i++){
    printf("copying old handler\n");
    new_handlers[i] = old_handlers[i];
  }
  printf("added old handlers\n");
  new_handlers[number_handlers-1] = new_handler;
  *pointer_to_handlers = new_handlers;

  if(old_handlers == NULL){
    printf("adding multi_listener\n");
    switch(type){
      case TICK: tick_timer_service_subscribe(MINUTE_UNIT,multi_window_tick_handler); break;
      case BLUETOOTH: bluetooth_connection_service_subscribe(multi_window_bluetooth_handler); break;
      case BATTERY: battery_state_service_subscribe(multi_window_battery_handler); break;
    }
  }
  free(old_handlers);
}