#pragma once
typedef void* Pointer;
void pointer_array_add(Pointer** pointer_to_array, Pointer new_pointer, int* number_pointers);
int pointer_array_remove(Pointer** pointer_to_array, Pointer pointer_to_remove, int* number_pointers);
