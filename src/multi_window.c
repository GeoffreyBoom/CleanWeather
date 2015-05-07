#include <pebble.h>
#include "multi_window.h"

MultiWindow    default_multi_window = {NULL,NULL,NULL,NULL,NULL,NULL,MINUTE_UNIT,0,0,0,0,0,false,false,false};

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

