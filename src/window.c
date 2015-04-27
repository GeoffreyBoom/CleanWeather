#include <pebble.h>
#include "window.h"
Window* get_window(){
  static Window* window = NULL;
  if(window == NULL){
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
      .unload = exit_window,
    });
  }
  return window;
}

void exit_window(){
  Window* s_window = get_window();
  window_stack_remove(s_window, true);
  window_destroy(s_window);
}