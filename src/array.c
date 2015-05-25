#include <pebble.h>
#include "array.h"

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

int function_array_remove(Function** pointer_to_handlers, Function handler_to_remove, int number_handlers){
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
  return num_in;
}

//very generic code:
void pointer_array_add(Pointer** pointer_to_handlers, Pointer new_handler, int* number_handlers){
  Pointer* old_handlers = *pointer_to_handlers;
  Pointer* new_handlers = malloc(sizeof(Pointer)*(*number_handlers+1));
  for(int i = 0; i < *number_handlers; i++){
    new_handlers[i] = old_handlers[i];
  }
  new_handlers[*number_handlers] = new_handler;
  *pointer_to_handlers = new_handlers;
  (*number_handlers)++;
  free(old_handlers);
}

int pointer_array_remove(Pointer** pointer_to_handlers, Pointer handler_to_remove, int* number_handlers){
  Pointer* old_handlers = *pointer_to_handlers;
  //count number of times handler_to_remove occurs
  int num_in = 0;
  for(int i = 0; i < *number_handlers;i++){
    if(old_handlers[i] == handler_to_remove){
      num_in++;
    }
  }
  //make new handlers array the same size as old array, minus number of occurences of handler_to_remove
  Pointer* new_handlers = malloc(sizeof(Pointer)*(*number_handlers-num_in));
  //copy all but the handler_to_remove
  int new_index = 0;
  for(int i = 0; i < *number_handlers; i++){
    if(old_handlers[i] != handler_to_remove){
      new_handlers[new_index] = old_handlers[i];
      new_index++;
    }
  }
  *pointer_to_handlers = new_handlers;
  free(old_handlers);
  (*number_handlers)-=num_in;
  return num_in;
}