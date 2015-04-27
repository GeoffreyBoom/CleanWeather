#include "sub_window.h"
#include <stdio.h>

struct WindowSequence{
  SubWindow* main_window;
  SubWindow** sub_window_array;
  int num_sub_windows;
  int current_window;
}default_sequence = {NULL, NULL, 0,-1};
  
struct SubWindow{
  void (* init)(void* args, int num_args);
  void (* de_init)(void* args, int num_args);
  void (* service_subscribe)(void* args, int num_args);
  void (* service_unsubscribe) (void* args, int num_args);
  bool displayed;
}default_sub_window = {NULL,NULL,NULL,NULL, false};

SubWindow* sub_window_create(void (* init)(void* args, int num_args),
                             void (* de_init)(void* args, int num_args), 
                             void (* service_subscribe)(void* args, int num_args),
                             void (* service_unsubscribe) (void* args, int num_args)){
  SubWindow* sub_window = malloc(sizeof(SubWindow));
  *sub_window = default_sub_window;
  sub_window->init = init;
  sub_window->de_init = de_init;
  sub_window->service_subscribe = service_subscribe;
  sub_window->service_unsubscribe = service_unsubscribe;
  return sub_window;
}

void sub_window_delete(SubWindow* sub_window){
  sub_window->init = NULL;
  sub_window->de_init = NULL;
  sub_window->service_subscribe = NULL;
  sub_window->service_unsubscribe = NULL;
  free(sub_window);
}

//initial window is the index starting at 0, whereas num_sub_windows should be equal to 2 if there are 2 windows, etc.
WindowSequence* window_sequence_create(SubWindow* main_window ,SubWindow** sub_window_array, int num_sub_windows, int initial_sub_window){
  WindowSequence* sequence = malloc(sizeof(WindowSequence));
  *sequence = default_sequence;
  sequence->main_window = main_window;
  sequence->sub_window_array = sub_window_array;
  sequence->num_sub_windows = num_sub_windows;
  sequence->current_window = initial_sub_window;
  return sequence;
}

void window_sequence_delete(WindowSequence* sequence){
  for(int i = 0; i < sequence->num_sub_windows; i++){
    free(sequence->sub_window_array[i]);
    sequence->sub_window_array[i] = NULL;
  }
  free(sequence->sub_window_array);
  sequence->sub_window_array = NULL;
  free(sequence->main_window);
}

void window_sequence_add_sub_window(SubWindow* sub_window, WindowSequence* sequence){
  
  //new size is one larger
  int new_size = sequence->num_sub_windows+1;
  SubWindow** new_array = malloc(sizeof(SubWindow) * new_size);
  
  //copy over values from old array to new array
  for(int i = 0; i < sequence->num_sub_windows; i++){
    new_array[i] = sequence->sub_window_array[i];
    sequence->sub_window_array[i] =NULL; 
  }
  new_array[new_size-1]= sub_window;

  //free the old array
  free(sequence->sub_window_array);
  //assign new array to sequence
  sequence->sub_window_array = new_array;
}

void sub_window_display(SubWindow* sub_window){
  if(sub_window->init != NULL){
    sub_window->init(NULL, 0);
  }
  if(sub_window->service_subscribe != NULL){
    sub_window->service_subscribe(NULL, 0);
  }
  sub_window->displayed=true;
}

void sub_window_de_display(SubWindow* sub_window){
  if(sub_window->de_init != NULL){
    sub_window->de_init(NULL, 0);
  }
  if(sub_window->service_unsubscribe != NULL){
    sub_window->service_unsubscribe(NULL, 0);
  }
  sub_window->displayed=false;
}

void window_sequence_display_next(WindowSequence* sequence){
  sequence->current_window = (sequence->current_window+1)%sequence->num_sub_windows;
  int previous_index = sequence->current_window-1;
  if(previous_index == -1){
    previous_index = sequence->num_sub_windows-1;
  }
  SubWindow* previous_window = sequence->sub_window_array[previous_index];
  if(previous_window->displayed==true){  
    sub_window_de_display(previous_window);
    printf("%i <- current window previous window-> %i",sequence->current_window, previous_index);
  }
  sub_window_display(sequence->sub_window_array[sequence->current_window]);
}

void window_sequence_display_initial(WindowSequence* sequence){
  if(!sequence->main_window->displayed){
    sub_window_display(sequence->main_window);
    (sequence->main_window->displayed) = true;
  }
  if(!(sequence->sub_window_array[sequence->current_window])->displayed){
    sub_window_display(sequence->sub_window_array[sequence->current_window]);
    (sequence->sub_window_array[sequence->current_window])->displayed= true;
  }
}

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
  
  SubWindow*     window = sub_window_create(main_init, main_de_init, NULL, NULL);
  SubWindow* sub_window = sub_window_create(weather_init, weather_deinit, NULL, NULL);
  SubWindow* sub_window2 = sub_window_create(test_init, test_init, NULL, NULL);
  SubWindow** windows = malloc(sizeof(SubWindow*)*2);
  windows[1] = sub_window;
  windows[0] = sub_window2;
  WindowSequence* sequence = window_sequence_create(window, windows, 2, 0);
  window_sequence_display_initial(sequence);
  window_sequence_display_next(sequence);
  app_event_loop();
  sub_window_de_display(sequence->sub_window_array[sequence->current_window]);
}
