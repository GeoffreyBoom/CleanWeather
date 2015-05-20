#pragma once
#include <pebble.h>
#include <stdio.h>
#include "window.h"
  
struct SubWindow;

typedef struct SubWindow SubWindow;

struct SubWindow{
  void (* init)(void* args, int num_args);
  void (* de_init)(void* args, int num_args);
  void (* service_subscribe)(void* args, int num_args);
  void (* service_unsubscribe) (void* args, int num_args);
  Layer* (* get_layer)();
  bool displayed;
};


void sub_window_display(SubWindow* sub_window);

void sub_window_de_display(SubWindow* sub_window);

SubWindow* sub_window_create(void (* init)(void* args, int num_args),
                             void (* de_init)(void* args, int num_args), 
                             void (* service_subscribe)(void* args, int num_args),
                             void (* service_unsubscribe) (void* args, int num_args),
                             Layer* (* get_layer)());

void sub_window_delete(SubWindow* sub_window);