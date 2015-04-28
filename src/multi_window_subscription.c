#include <pebble.h>
#include "multi_window_subscription.h"
#include "stdio.h"

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
  multi_window_tick_service_subscribe(tick_subscription.tick_units, tick_subscription.tick_handler);
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

void multi_window_tick_service_subscribe(TimeUnits tick_units, TickHandler tick_handler){
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
    printf("setting up a time listener\n");
    multi_window->tick_in_use = true;
    int number_tick_handlers = multi_window->number_tick_handlers;
    
    TickHandler* old_tick_handlers = multi_window->tick_handlers;
    TickHandler* new_tick_handlers = malloc(sizeof(TickHandler)*(number_tick_handlers+1));
    for(int i = 0; i < number_tick_handlers; i++){
      new_tick_handlers[i] = old_tick_handlers[i];
    }
    new_tick_handlers[number_tick_handlers] = tick_handler;
    multi_window->tick_handlers = new_tick_handlers;
    multi_window->number_tick_handlers += 1;
    
    if(old_tick_handlers == NULL){
      tick_timer_service_subscribe(multi_window->time_units,multi_window_tick_handler);
    }
    free(old_tick_handlers);
    
    multi_window->tick_in_use = false;
  }
}

void multi_window_bluetooth_connection_service_subscribe(BluetoothConnectionHandler BT_handler){
  MultiWindow* multi_window = get_multi_window();
  multi_window->number_bluetooth_handlers+=1;
  int number_handlers = multi_window->number_bluetooth_handlers;
  BluetoothConnectionHandler* old_handlers = multi_window->bluetooth_connection_handlers;
  BluetoothConnectionHandler* new_handlers = malloc(sizeof(TickHandler)*(number_handlers));
  for(int i = 0; i < number_handlers-1; i++){
    new_handlers[i] = old_handlers[i];
  }
  
  new_handlers[number_handlers-1] = BT_handler;
  multi_window->bluetooth_connection_handlers = new_handlers;

  if(old_handlers == NULL){
    bluetooth_connection_service_subscribe(multi_window_bluetooth_handler);
  }
  free(old_handlers);

}

void multi_window_battery_state_service_subscribe(BatteryStateHandler BS_handler){
  MultiWindow* multi_window = get_multi_window();
  multi_window->number_battery_handlers+=1;
  int number_handlers = multi_window->number_battery_handlers;
  BatteryStateHandler* old_handlers = multi_window->battery_state_handlers;    
  BatteryStateHandler* new_handlers = malloc(sizeof(TickHandler)*(number_handlers));
  for(int i = 0; i < number_handlers-1; i++){
    new_handlers[i] = old_handlers[i];
  }

  new_handlers[number_handlers-1] = BS_handler;
  multi_window->battery_state_handlers = new_handlers;
  
  if(old_handlers == NULL){
    //battery_state_service_subscribe(multi_window->time_units,multi_window_tick_handler);
  }
  free(old_handlers);
}
