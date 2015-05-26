#include <pebble.h>
#include "array.h"

void pointer_array_add(Pointer** pointer_to_array, Pointer new_pointer, int* number_pointers){
  Pointer* old_array = *pointer_to_array;
  Pointer* new_array = malloc(sizeof(Pointer)*((*number_pointers)+1));
  for(int i = 0; i < *number_pointers;i++){
    new_array[i] = old_array[i];
  }
  new_array[*number_pointers] = new_pointer;
  *pointer_to_array = new_array;
  (*number_pointers)++;
  free(old_array);
}
int pointer_array_remove(Pointer** pointer_to_array, Pointer pointer_to_remove, int* number_pointers){
  Pointer* old_array = *pointer_to_array;
  int num_in = 0;
  for(int i = 0; i < *number_pointers; i++){
    if(old_array[i] == pointer_to_remove){
      num_in++;
    }
  }
  Pointer* new_array = malloc(sizeof(Pointer)*((*number_pointers)-num_in));
  int index = 0;
  for(int i = 0; i < *number_pointers; i++){
    if(old_array[i] != pointer_to_remove){
      new_array[index++] = old_array[i];
    }
  }
  *pointer_to_array = new_array;
  (*number_pointers)-= num_in;
  free(old_array);
  return num_in;
}