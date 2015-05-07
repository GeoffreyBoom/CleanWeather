#include <pebble.h>

#include <pebble.h>
#include <stdio.h>
#include "main_window.h"


TextLayer *title_layer;
TextLayer *time_layer;
TextLayer *month_layer;
TextLayer *weekday_layer;
TextLayer *year_layer;

BitmapLayer *bluetooth_layer = NULL;
GBitmap *bluetooth_icon = NULL;
BitmapLayer *battery_layer = NULL;
GBitmap *battery_icon = NULL;
BitmapLayer *battery_charging_layer = NULL;
GBitmap *battery_charging_icon = NULL;
int battery_level = 10;

void init(void) {

  show_window();
  
  // Begin Clock Ticking
  multi_window_tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //begin bluetooth service
  multi_window_bluetooth_connection_service_subscribe(bluetooth_handler);
  //initializing bluetooth state
  bluetooth_handler(bluetooth_connection_service_peek());
  
  //begin battery service
  multi_window_battery_state_service_subscribe(battery_handler);
  //initializing battery
  battery_handler(battery_state_service_peek());
}

void deinit(void) {
  // Destroy main Window
  hide_window();
  //window_destroy(s_main_window);
  bluetooth_connection_service_unsubscribe();
  // Finish using AppSync
  gbitmap_destroy(bluetooth_icon);
  bitmap_layer_destroy(bluetooth_layer);
}

void bluetooth_handler(bool bluetooth){
  Window* s_window = get_window();
  if(bluetooth){
    if(bluetooth_icon == NULL){
      bluetooth_icon = gbitmap_create_with_resource(RESOURCE_ID_bluetooth_dark_icon);
    }
    if(bluetooth_layer == NULL){
      bluetooth_layer = bitmap_layer_create(GRect(125+6,2,8,13));
    }
    bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_icon);
    layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(bluetooth_layer));
  }
  else{
    printf("bluetooth off");
    if(bluetooth_layer != NULL){
      layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer));
      bitmap_layer_destroy(bluetooth_layer);
      bluetooth_layer = NULL;
    }
    if(bluetooth_icon != NULL){
      //gbitmap_destroy(bluetooth_icon);
      //bluetooth_icon = NULL;
    }
  }
}
/*
int main(void) {
  init();
  app_event_loop();
  deinit();
}
*/

void draw_battery_power(Layer *layer, GContext *ctx) {
  graphics_draw_bitmap_in_rect(ctx, battery_icon, layer_get_bounds(layer));
  int top = 3;
  int left = 4;
  graphics_context_set_text_color(ctx, GColorBlack);
  for(int row = top; row < top+4; row++){
    for(int col = left; col < left + battery_level; col++){
      graphics_draw_pixel(ctx, GPoint(col, row));
    }
  }
}

void battery_handler(BatteryChargeState battery){
  Window* s_window = get_window();
  float percent = battery.charge_percent;
  int top = 4;
  int left = 5;
  battery_level = percent/10;
  if(battery_icon == NULL && battery_layer == NULL){
    battery_icon = gbitmap_create_with_resource(RESOURCE_ID_battery_dark_icon);
    battery_layer = bitmap_layer_create(GRect(left-1,top-1,19,10));
    bitmap_layer_set_bitmap(battery_layer, battery_icon);
    layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(battery_layer));
    layer_set_update_proc(bitmap_layer_get_layer(battery_layer), draw_battery_power);
  }
  layer_mark_dirty(bitmap_layer_get_layer(battery_layer));
  if (battery.is_charging){
    if(battery_charging_icon == NULL|| battery_charging_layer == NULL){
      if(battery_charging_icon == NULL){
        battery_charging_icon = gbitmap_create_with_resource(RESOURCE_ID_CHARGING_DARK_ICON);
      }
      if(battery_charging_layer == NULL){
        battery_charging_layer = bitmap_layer_create(GRect(125-6,2,7,14));
      }
      bitmap_layer_set_compositing_mode(battery_charging_layer ,GCompOpAnd);
      bitmap_layer_set_bitmap(battery_charging_layer, battery_charging_icon);
      layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(battery_charging_layer));
    }
  }
  else {
    printf("charger off");
    if(battery_charging_layer != NULL){
      layer_remove_from_parent(bitmap_layer_get_layer(battery_charging_layer));
      bitmap_layer_destroy(battery_charging_layer);
      battery_charging_layer = NULL;
    }
    if(battery_charging_icon != NULL){
      //gbitmap_destroy(battery_charging_icon);
      //battery_charging_icon = NULL;
    }
  }
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed ){
  time_t temp = (time(NULL));
  tick_time = localtime(&temp);
  static char dtime[]   = "00:00";
  static char month[]   = "--- 00";
  static char weekday[] = "---";
  static char date[]    = "00";
  static char year[]    = "0000";
  //set time
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(dtime,  sizeof("00:00"), "%R"    , tick_time);
  } else {
    // Use 12 hour format
    strftime(dtime,  sizeof("00:00"), "%I:%M", tick_time);
  }
  
  //set month
  strftime(month,   sizeof("--- 00"),   "%b %d", tick_time);
  
  //set weekday
  strftime(weekday, sizeof("---"),   "%a"    , tick_time);
  
  //set date
  strftime(date,    sizeof("00"),    "%d"    , tick_time);
  //set_text_date(date);
  
  //set year
  strftime(year,    sizeof("0000"),    "%Y"    , tick_time);
  set_text_year(year);
  set_text_month(month);
  set_text_weekday(weekday);
  set_text_time(dtime);
}

static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static GFont s_res_bitham_30_black;


void initialise_ui(void) {
  Window* s_window = get_window();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  
  // title_layer
  title_layer = text_layer_create(GRect(0, 0, 144, 16));
  text_layer_set_text(title_layer, "CleanWeather");
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)title_layer);
  
  // time_layer
  time_layer = text_layer_create(GRect(20, 37, 104, 31));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text(time_layer, "12:12");
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)time_layer);
  
  // month_layer
  month_layer = text_layer_create(GRect(85, 14, 60, 30));
  text_layer_set_background_color(month_layer, GColorClear);
  text_layer_set_text_color(month_layer, GColorWhite);
  text_layer_set_text(month_layer, "apr 23");
  text_layer_set_text_alignment(month_layer, GTextAlignmentRight);
  text_layer_set_font(month_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)month_layer);

  // year_layer
  year_layer = text_layer_create(GRect(4, 14, 42, 28));
  text_layer_set_background_color(year_layer, GColorClear);
  text_layer_set_text_color(year_layer, GColorWhite);
  text_layer_set_text(year_layer, "2015");
  text_layer_set_text_alignment(year_layer, GTextAlignmentCenter);
  text_layer_set_font(year_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)year_layer);
  
  // weekday_layer
  weekday_layer = text_layer_create(GRect(54, 14, 33, 30));
  text_layer_set_background_color(weekday_layer, GColorClear);
  text_layer_set_text_color(weekday_layer, GColorWhite);
  text_layer_set_text(weekday_layer, "Wed");
  text_layer_set_text_alignment(weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(weekday_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)weekday_layer);
}

void destroy_ui(void) {
  Window* s_window = get_window();
  window_destroy(s_window);
  text_layer_destroy(title_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(month_layer);
  text_layer_destroy(weekday_layer);
  text_layer_destroy(year_layer);
}

void handle_window_unload(Window* window) {
  destroy_ui();
}

void set_text_title(char* title){
  text_layer_set_text(title_layer, title);
}

//time functions
void set_text_year(char* year){
  text_layer_set_text(year_layer, year);
}
void set_text_month(char* month){
  text_layer_set_text(month_layer, month);
}
void set_text_weekday(char* weekday){
  text_layer_set_text(weekday_layer, weekday);
}
void set_text_time(char* time){
  text_layer_set_text(time_layer, time);
}

void show_window(void) {
  Window* s_window = get_window();

  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_window(void) {
  Window* s_window = get_window();
  window_stack_remove(s_window, true);
}

//=======================ENDWINDOW=====================//