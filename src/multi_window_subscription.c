#include <pebble.h>
#include <stdio.h>
#include "multi_window_subscription.h"
#include "stdio.h"
enum handler_types{
  TICK = 0,
  BLUETOOTH=1,
  BATTERY=2,
  TAP=3,
  SYNC=4
};

typedef void (*Function)();
void add_handler(Function** pointer_to_handlers, Function new_handler, int number_handlers, int type);
void remove_handler(Function** pointer_to_handlers, Function handler_to_remove, int number_handlers, int type);
void function_array_add(Function** pointer_to_handlers, Function new_handler, int number_handlers);
void function_array_remove(Function** pointer_to_handlers, Function handler_to_remove, int number_handlers);

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

void multi_window_app_sync_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context){
  MultiWindow* multi_window = get_multi_window();
  if(multi_window->sync_handlers != NULL){
    for(int i = 0; i < multi_window->number_sync_handlers; i++){
      multi_window->sync_handlers[i](key, new_tuple, old_tuple, context);
    }
  }
}

void multi_window_appsync_init(struct AppSync * s, 
                               uint8_t * buffer, 
                               const uint16_t buffer_size, 
                               const Tuplet *const keys_and_initial_values, 
                               const uint8_t count,
                               void * context){
  app_sync_init(s, buffer, buffer_size, keys_and_initial_values, count, multi_window_app_sync_handler, NULL, context);
}

void multi_window_app_sync_deinit(struct AppSync * s){
  app_sync_deinit(s);
}



//subscription functions:
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
    add_handler((Function**)&(multi_window->tick_handlers), (Function) tick_handler, multi_window->number_tick_handlers, TICK);
    multi_window->number_tick_handlers+=1;
    
    multi_window->tick_in_use = false;
  }
}

//todo: implement synchronization for the other types of handlers.
void multi_window_bluetooth_connection_service_subscribe(BluetoothConnectionHandler BT_handler){
  MultiWindow* multi_window = get_multi_window();
  add_handler((Function**)&(multi_window->bluetooth_connection_handlers), (Function) BT_handler, multi_window->number_bluetooth_handlers, BLUETOOTH);
  multi_window->number_bluetooth_handlers+=1;
}

void multi_window_battery_state_service_subscribe(BatteryStateHandler BS_handler){
  MultiWindow* multi_window = get_multi_window();
  add_handler(&(multi_window->battery_state_handlers), BS_handler, multi_window->number_battery_handlers, BATTERY);
  multi_window->number_battery_handlers+=1;
}

void multi_window_accel_tap_service_subscribe(AccelTapHandler tap_handler){
  MultiWindow* multi_window = get_multi_window();
  add_handler((Function**)&(multi_window->tap_handlers), (Function)tap_handler, multi_window->number_tap_handlers, TAP);
  multi_window->number_tap_handlers+=1;
}

void multi_window_app_sync_service_subscribe(AppSyncTupleChangedCallback change_handler){
  MultiWindow* multi_window = get_multi_window();
  add_handler((Function**)&(multi_window->sync_handlers), (Function) change_handler, multi_window->number_sync_handlers, SYNC);
  multi_window->number_sync_handlers+=1;
}

//unsubscription functions

void multi_window_tick_timer_service_unsubscribe(TickHandler tick_handler){
  MultiWindow* multi_window = get_multi_window();
  remove_handler((Function**)&(multi_window->tick_handlers), (Function) tick_handler, multi_window->number_tick_handlers, TICK);
  multi_window->number_tick_handlers-=1;
}
void multi_window_bluetooth_connection_service_unsubscribe(BluetoothConnectionHandler BT_handler){
  MultiWindow* multi_window = get_multi_window();
  remove_handler((Function**)&(multi_window->bluetooth_connection_handlers), (Function) BT_handler, multi_window->number_bluetooth_handlers, BLUETOOTH);
  multi_window->number_bluetooth_handlers-=1;
}

void multi_window_battery_state_service_unsubscribe(BatteryStateHandler BS_handler){
  MultiWindow* multi_window = get_multi_window();
  remove_handler(&(multi_window->battery_state_handlers), BS_handler, multi_window->number_battery_handlers, BATTERY);
  multi_window->number_battery_handlers-=1;
}

void multi_window_accel_tap_service_unsubscribe(AccelTapHandler tap_handler){
  MultiWindow* multi_window = get_multi_window();
  remove_handler((Function**)&(multi_window->tap_handlers), (Function)tap_handler, multi_window->number_tap_handlers, TAP);
  multi_window->number_tap_handlers-=1;
}

void multi_window_app_sync_service_unsubscribe(AppSyncTupleChangedCallback change_handler){
  MultiWindow* multi_window = get_multi_window();
  remove_handler((Function**)&(multi_window->tap_handlers), (Function) change_handler, multi_window->number_tap_handlers, SYNC);
  multi_window->number_sync_handlers-=1;
}

//generic code:

void add_handler(Function** pointer_to_handlers, Function new_handler, int number_handlers, int type){
  function_array_add(pointer_to_handlers, new_handler, number_handlers);
  if(number_handlers==0){
    printf("adding multi_listener\n");
    switch(type){
      case TICK: tick_timer_service_subscribe(MINUTE_UNIT,multi_window_tick_handler); break;
      case BLUETOOTH: bluetooth_connection_service_subscribe(multi_window_bluetooth_handler); break;
      case BATTERY: battery_state_service_subscribe(multi_window_battery_handler); break;
      case TAP: accel_tap_service_subscribe(multi_window_tap_handler); break;
    }
  }
}

void remove_handler(Function** pointer_to_handlers, Function handler_to_remove, int number_handlers, int type){
  function_array_remove(pointer_to_handlers, handler_to_remove, number_handlers);
  if (number_handlers == 1){
    switch(type){
      case TICK: tick_timer_service_unsubscribe(); break;
      case BLUETOOTH: bluetooth_connection_service_unsubscribe(); break;
      case BATTERY: battery_state_service_unsubscribe(); break;
      case TAP: accel_tap_service_unsubscribe(); break;
    }
  }
}


//very generic code:
void function_array_add(Function** pointer_to_handlers, Function new_handler, int number_handlers){
  Function* old_handlers = *pointer_to_handlers;
  Function* new_handlers = malloc(sizeof(Function)*(number_handlers+1));
  for(int i = 0; i < number_handlers; i++){
    new_handlers[i] = old_handlers[i];
  }
  new_handlers[number_handlers] = new_handler;
  *pointer_to_handlers = new_handlers;
  free(old_handlers);
}

void function_array_remove(Function** pointer_to_handlers, Function handler_to_remove, int number_handlers){
  Function* old_handlers = *pointer_to_handlers;
  //count number of times handler_to_remove occurs
  int num_in = 0;
  for(int i = 0; i < number_handlers;i++){
    if(old_handlers[i] == handler_to_remove){
      num_in++;
    }
  }
  //make new handlers array the same size as old array, minus number of occurences of handler_to_remove
  Function* new_handlers = malloc(sizeof(Function)*(number_handlers-num_in));
  //copy all but the handler_to_remove
  int new_index = 0;
  for(int i = 0; i < number_handlers; i++){
    if(old_handlers[i] != handler_to_remove){
      new_handlers[new_index] = old_handlers[i];
      new_index++;
    }
  }
  *pointer_to_handlers = new_handlers;
  free(old_handlers);
}