#include "main.h"
//EXAMPLE CODE//

void main_init(void* nothing, int none){
  init();
}
void main_de_init(void* nothing, int none){
  deinit();
}

void weather_init(void* nothing, int none){
  weather_neat_init();
}

void weather_deinit(void* nothing, int none){
  weather_neat_deinit();
}
void test_init(void* nothing, int none){

}
void test_deinit(void* nothing, int none){
  
}

int main(void){
  SubWindow*     window = sub_window_create(main_init, main_de_init,NULL, NULL, NULL);
  SubWindow* sub_window = sub_window_create(weather_init, weather_deinit, NULL, NULL, NULL);
  SubWindow* sub_window2 = sub_window_create(test_init, test_init, NULL, NULL, NULL);
  SubWindow** windows = malloc(sizeof(SubWindow*)*2);
  windows[1] = sub_window;
  windows[0] = sub_window2;
  WindowSequence* sequence = window_sequence_create(window, windows, 2, 0);
  window_sequence_display_initial(sequence);
  window_sequence_display_next(sequence);
  app_event_loop();
  sub_window_de_display(sequence->sub_window_array[sequence->current_window]);
}