#include <pebble.h>
#include "multi_window.h"

MultiWindow    default_multi_window = {NULL,NULL,NULL,NULL,NULL,NULL,MINUTE_UNIT,0,0,0,0,0,false,false,false};

int delta_time_shake = 5;

//retrieves the multi_window singleton.
MultiWindow* get_multi_window(){
  static MultiWindow* multi_window = NULL;
  if(multi_window == NULL){
    multi_window = malloc(sizeof(MultiWindow));
    *multi_window = default_multi_window;
  }
  return multi_window;
}

void multi_window_set_main_window(SubWindow* main_window){
  MultiWindow* multi_window = get_multi_window();
  if(multi_window->window_sequence==NULL){
    multi_window->window_sequence = window_sequence_create(NULL, NULL, 0,0);
  }
  multi_window->window_sequence->main_window = main_window;
}
void multi_window_add_sub_window(SubWindow* new_window){
  MultiWindow* multi_window = get_multi_window();
  if(multi_window->window_sequence==NULL){
    multi_window->window_sequence = window_sequence_create(NULL,NULL,0,0);
  }
  window_sequence_add_sub_window(new_window, multi_window->window_sequence);
}
void multi_window_display_initial(){
  MultiWindow* multi_window = get_multi_window();
  window_sequence_display_initial(multi_window->window_sequence);
}
void multi_window_display_next(){
  MultiWindow* multi_window = get_multi_window();
  window_sequence_display_next(multi_window->window_sequence);
}

void shake_for_next(AccelAxisType axis, int32_t direction){
  static time_t timestamp = 0;
  time_t curr = time(NULL);
  printf("shake happened");
  if(timestamp==0 || ((curr-timestamp)>delta_time_shake)){
    printf("dt greater than %i: %lu", delta_time_shake, curr-timestamp);
    timestamp = curr;
  }
  else if((time(NULL)-timestamp)<delta_time_shake){
    printf("dt less than %i: %lu", delta_time_shake, curr-timestamp);
    timestamp = curr;
    multi_window_display_next();
  }
}

void multi_window_shake_for_next(bool enabled){
  if(enabled){
    multi_window_accel_tap_service_subscribe(shake_for_next);
  }
  else{
    multi_window_accel_tap_service_unsubscribe(shake_for_next);
  }
}
