#include <pebble.h>

static TextLayer *title_layer;
static TextLayer *time_layer;
static TextLayer *month_layer;
static TextLayer *temperature_layer;
static TextLayer *weekday_layer;
static TextLayer *condition_layer;
static TextLayer *year_layer;
static TextLayer *location_layer;
// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, 1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

//time functions
static void set_text_year(char* year){
  text_layer_set_text(year_layer, year);
}
static void set_text_month(char* month){
  text_layer_set_text(month_layer, month);
}
static void set_text_weekday(char* weekday){
  text_layer_set_text(weekday_layer, weekday);
}
static void set_text_time(char* time){
  text_layer_set_text(time_layer, time);
}

//weather functions
static void set_text_temperature(char* temperature){
  text_layer_set_text(temperature_layer, temperature);
}
static void set_text_condition(char* condition){
  text_layer_set_text(condition_layer, condition);
}
static void set_text_location(char* location){
  text_layer_set_text(location_layer, location);
}

void show_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_window(void) {
  window_stack_remove(s_window, true);
}

//=======================ENDWINDOW=====================//