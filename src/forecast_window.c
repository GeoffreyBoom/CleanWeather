#include <pebble.h>
#include "forecast_window.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
 Layer *layer;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_3;
static TextLayer *s_textlayer_4;

static void initialise_ui(void) {
  Weather* weather = get_weather_buffer();
  
  layer = layer_create(GRect(0, 75, 144, 144));
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(2, 65-65, 140, 20));
  text_layer_set_text(s_textlayer_1, weather->forecast_1);
  layer_add_child(layer, (Layer *)s_textlayer_1);
  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(2, 90-65, 140, 20));
  text_layer_set_text(s_textlayer_2, weather->forecast_2);
  layer_add_child(layer, (Layer *)s_textlayer_2);
  
  // s_textlayer_3
  s_textlayer_3 = text_layer_create(GRect(2, 115-65, 140, 20));
  text_layer_set_text(s_textlayer_3, weather->forecast_3);
  layer_add_child(layer, (Layer *)s_textlayer_3);  
  // s_textlayer_4
  s_textlayer_4 = text_layer_create(GRect(2, 115+25-65, 140, 20));
  text_layer_set_text(s_textlayer_4, weather->forecast_4);
  layer_add_child(layer, (Layer *)s_textlayer_4);  
}

static void destroy_ui(void) {
  layer_destroy(layer);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_textlayer_3);
  text_layer_destroy(s_textlayer_4);
  s_textlayer_1 = NULL;
  s_textlayer_2 = NULL;
  s_textlayer_3 = NULL;
  s_textlayer_4 = NULL;

  layer = NULL;
}
// END AUTO-GENERATED UI CODE

void show_forecast_window(void) {
  initialise_ui();
}

void hide_forecast_window(void) {
  destroy_ui();
}

void set_text_forecast_1(char* text){
  if(s_textlayer_1){
    text_layer_set_text(s_textlayer_1, text);
  }
}
void set_text_forecast_2(char* text){
  if(s_textlayer_2){
    text_layer_set_text(s_textlayer_2, text);
  }
}
void set_text_forecast_3(char* text){
  if(s_textlayer_3){
    text_layer_set_text(s_textlayer_3, text);
  }
}
void set_text_forecast_4(char* text){
  if(s_textlayer_4){
    text_layer_set_text(s_textlayer_3, text);
  }
}


Layer* get_forecast_layer(){
  return layer;
}