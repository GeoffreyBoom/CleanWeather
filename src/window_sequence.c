#include <pebble.h>
#include "window_sequence.h"
  
WindowSequence default_sequence = {NULL, NULL, 0,-1};


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
  int new_size = sequence->num_sub_windows+=1;
  SubWindow** new_array = malloc(sizeof(SubWindow) * new_size);
  
  //copy over values from old array to new array
  for(int i = 0; i < sequence->num_sub_windows-1; i++){
    new_array[i] = sequence->sub_window_array[i];
    sequence->sub_window_array[i] =NULL; 
  }
  new_array[new_size-1]= sub_window;

  //free the old array
  free(sequence->sub_window_array);
  //assign new array to sequence
  sequence->sub_window_array = new_array;
}

void window_sequence_remove_sub_window(SubWindow* subwindow, WindowSequence* sequence){
  sub_window_de_display(sequence->sub_window_array[sequence->current_window]);
  pointer_array_remove((Pointer**)&sequence->sub_window_array, subwindow, &sequence->num_sub_windows);
  if(sequence->num_sub_windows > 0){
    sequence->current_window = 0;
    sub_window_display(sequence->sub_window_array[sequence->current_window]);
  }
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