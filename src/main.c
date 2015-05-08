#include "main.h"
#include "stdio.h"
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
  multi_window_set_main_window(sub_window_create(main_init, main_de_init,NULL, NULL, NULL));
  multi_window_add_sub_window(sub_window_create(test_init, test_init, NULL, NULL, NULL));
  multi_window_add_sub_window(sub_window_create(weather_init, weather_deinit, NULL, NULL, NULL));
  
  multi_window_display_initial();
  multi_window_display_next();
  app_event_loop();
  //sub_window_de_display(sequence->sub_window_array[sequence->current_window]);
}