#include "sub_window.h"

SubWindow default_sub_window   = {NULL,NULL,NULL,NULL,NULL, false};

SubWindow* sub_window_create(void (* init)(void* args, int num_args),
                             void (* de_init)(void* args, int num_args), 
                             void (* service_subscribe)(void* args, int num_args),
                             void (* service_unsubscribe) (void* args, int num_args),
                             Layer* layer){
  SubWindow* sub_window = malloc(sizeof(SubWindow));
  *sub_window = default_sub_window;
  sub_window->init = init;
  sub_window->de_init = de_init;
  sub_window->service_subscribe = service_subscribe;
  sub_window->service_unsubscribe = service_unsubscribe;
  sub_window->layer=layer;
  return sub_window;
}

void sub_window_delete(SubWindow* sub_window){
  sub_window->init = NULL;
  sub_window->de_init = NULL;
  sub_window->service_subscribe = NULL;
  sub_window->service_unsubscribe = NULL;
  sub_window->layer=NULL;
  free(sub_window);
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



