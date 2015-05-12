#pragma once
#include "sub_window.h"

struct WindowSequence;

typedef struct WindowSequence WindowSequence;

void window_sequence_add_sub_window(SubWindow* sub_window, WindowSequence* sequence);

void window_sequence_display_next(WindowSequence* sequence);

void window_sequence_display_initial(WindowSequence* sequence);

void window_sequence_delete(WindowSequence* sequence);

WindowSequence* window_sequence_create(SubWindow* main_window ,SubWindow** sub_window_array, int num_sub_windows, int initial_sub_window);

struct WindowSequence{
  SubWindow* main_window;
  SubWindow** sub_window_array;
  int num_sub_windows;
  int current_window;
};