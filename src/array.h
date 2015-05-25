#pragma once
typedef void (*Function)();
typedef void* Pointer;

void pointer_array_add(Pointer** pointer_to_handlers, Pointer new_handler, int* number_handlers);
int pointer_array_remove(Pointer** pointer_to_handlers, Pointer handler_to_remove, int* number_handlers);
void function_array_add(Function** pointer_to_handlers, Function new_handler, int number_handlers);
int function_array_remove(Function** pointer_to_handlers, Function handler_to_remove, int number_handlers);