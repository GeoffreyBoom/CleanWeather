#include <pebble.h>
#include "forecast_window.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
 Layer *layer;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_3;

static void initialise_ui(void) {
  //Window* s_window = get_window();
  layer = layer_create(GRect(0, 75, 144, 144));
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(2, 65-65, 140, 20));
  text_layer_set_text(s_textlayer_1, " 00:00 / 0°C chance of rain");
  layer_add_child(layer, (Layer *)s_textlayer_1);
  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(2, 90-65, 140, 20));
  text_layer_set_text(s_textlayer_2, " 00:00 / 0°C chance of rain");
  layer_add_child(layer, (Layer *)s_textlayer_2);
  
  // s_textlayer_3
  s_textlayer_3 = text_layer_create(GRect(2, 115-65, 140, 20));
  text_layer_set_text(s_textlayer_3, " 00:00 / 0°C chance of rain");
  layer_add_child(layer, (Layer *)s_textlayer_3);
  
  //layer_add_child(window_get_root_layer(s_window), layer);
  
}

static void destroy_ui(void) {
  layer_destroy(layer);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_textlayer_3);
  s_textlayer_1 = NULL;
  s_textlayer_2 = NULL;
  s_textlayer_3 = NULL;
  layer = NULL;
}
// END AUTO-GENERATED UI CODE

void show_forecast_window(void) {
  initialise_ui();
}

void hide_forecast_window(void) {
  destroy_ui();
}

Layer* get_forecast_layer(){
  return layer;
}