#include <pebble.h>
#include "main.h"


static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // Update the TextLayer output
  static char s_city_buffer[32];
  static char s_temperature_buffer[5];
  static char s_condition_buffer[32];
  switch(key){
    case WEATHER_CITY_KEY:
      snprintf(s_city_buffer, sizeof(s_city_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer.location, s_city_buffer, sizeof(s_city_buffer));
      set_text_location(s_city_buffer);
      break;
    case WEATHER_TEMPERATURE_KEY:
      snprintf(s_temperature_buffer, sizeof(s_temperature_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer.temperature, s_temperature_buffer, sizeof(s_temperature_buffer));
      set_text_temperature(s_temperature_buffer);
      break;
    case WEATHER_CONDITION_KEY:
      snprintf(s_condition_buffer, sizeof(s_condition_buffer), "%s", (char*)new_tuple->value->cstring);
      strncpy(weather_buffer.condition, s_condition_buffer, sizeof(s_condition_buffer));
      set_text_condition(s_condition_buffer);
      break;
  }
  weather_buffer.time = time(NULL);
  persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));
  tick_handler(NULL, MINUTE_UNIT);
}

void request_weather(void){

  DictionaryIterator *iter;
	
  app_message_outbox_begin(&iter);
  
  dict_write_end(iter);
  
  app_message_outbox_send();
  
}

static void weather_callback(void* data){
  request_weather();
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
  switch (app_message_error) {
    case APP_MSG_OK:   APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_OK"); break;
    case APP_MSG_SEND_TIMEOUT: 
      APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_SEND_TIMEOUT");
      app_timer_register(1000, weather_callback, NULL);
      break;
    case APP_MSG_SEND_REJECTED: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_SEND_REJECTED");break;
    case APP_MSG_NOT_CONNECTED: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_NOT_CONNECTED");break;
    case APP_MSG_APP_NOT_RUNNING: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_APP_NOT_RUNNING");break;
    case APP_MSG_INVALID_ARGS: APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_INVALID_ARGS");break;
    case APP_MSG_BUSY:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_BUSY");break;
    case APP_MSG_BUFFER_OVERFLOW:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_BUFFER_OVERFLOW"); break;
    case APP_MSG_ALREADY_RELEASED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_ALREADY_RELEASED"); break;
    case APP_MSG_CALLBACK_ALREADY_REGISTERED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_CALLBACK_ALREADY_REGISTERED");break;
    case APP_MSG_CALLBACK_NOT_REGISTERED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_CALLBACK_NOT_REGISTERED"); break;
    case APP_MSG_OUT_OF_MEMORY:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_OUT_OF_MEMORY");break;
    case APP_MSG_CLOSED:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_CLOSED"); break;
    case APP_MSG_INTERNAL_ERROR:APP_LOG(APP_LOG_LEVEL_ERROR, "APP_MSG_INTERNAL_ERROR");break;
    default:APP_LOG(APP_LOG_LEVEL_ERROR, "UNKNOWN ERROR"); break;
  }
}

static void light_off(void* data){
  set_text_title("CleanWeather");
  light_enable(false);
}


static void shake_handler(AccelAxisType axis, int32_t direction){
  light_enable(true);
  app_timer_register(1000*light_time, light_off, NULL);
  set_text_title("Light!");
}

static void init(void) {
  show_window();
  
  // Begin App Sync
  setup_app_sync();
  
  // Begin Clock Ticking
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //register shake handler
  accel_tap_service_subscribe(shake_handler);

  //begin bluetooth service
  bluetooth_connection_service_subscribe(bluetooth_handler);
  //initializing bluetooth state
  bluetooth_handler(bluetooth_connection_service_peek());
  
  //begin battery service
  battery_state_service_subscribe(battery_handler);
  //initializing battery
  battery_handler(battery_state_service_peek());

  
  // Start Weather Updating
  start_weather_timer(NULL);
}

static void setup_app_sync(){
  // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  //load initial values
  //if value not in memory
  //create default initial values
  if(!(persist_exists(WEATHER_DATA_LOCATION)
     && persist_read_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather)) != E_DOES_NOT_EXIST)){
    strcpy(weather_buffer.location, "NULL");
    strcpy(weather_buffer.temperature, "NULL°C");
    strcpy(weather_buffer.condition, "NULL");
    weather_buffer.time = time(NULL);
    persist_write_data(WEATHER_DATA_LOCATION, &weather_buffer, sizeof(struct Weather));
  }
  Tuplet initial_values[] = {
    TupletCString(WEATHER_CITY_KEY, weather_buffer.location),
    TupletCString(WEATHER_TEMPERATURE_KEY, weather_buffer.temperature),
    TupletCString(WEATHER_CONDITION_KEY, weather_buffer.condition),
    TupletInteger(LIGHT_TIME_KEY, 6)
  };
  
  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

static void start_weather_timer(void* data){
  request_weather();
  app_timer_register(1000*60*10, start_weather_timer, NULL);
}

static void deinit(void) {
  // Destroy main Window
  hide_window();
  //window_destroy(s_main_window);
  bluetooth_connection_service_unsubscribe();
  // Finish using AppSync
  app_sync_deinit(&s_sync);
  gbitmap_destroy(bluetooth_icon);
  bitmap_layer_destroy(bluetooth_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

static void draw_battery_power(Layer *layer, GContext *ctx) {
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

static void battery_handler(BatteryChargeState battery){
  float percent = battery.charge_percent;
  int top = 4;
  int left = 5;
  if(battery_icon == NULL && battery_layer == NULL){
    battery_icon = gbitmap_create_with_resource(RESOURCE_ID_battery_dark_icon);
    battery_layer = bitmap_layer_create(GRect(left-1,top-1,19,10));
    bitmap_layer_set_bitmap(battery_layer, battery_icon);
    layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(battery_layer));
    layer_set_update_proc(bitmap_layer_get_layer(battery_layer), draw_battery_power);
  }
  layer_mark_dirty(bitmap_layer_get_layer(battery_layer));
  battery_level = percent/10;
}

static void bluetooth_handler(bool bluetooth){
  if(bluetooth){
    bluetooth_icon = gbitmap_create_with_resource(RESOURCE_ID_bluetooth_dark_icon);
    bluetooth_layer = bitmap_layer_create(GRect(125,2,8,13));
    bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_icon);
    layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(bluetooth_layer));
  }
  else{
    bitmap_layer_destroy(bluetooth_layer);
    gbitmap_destroy(bluetooth_icon);
  }
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed ){
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
  strftime(month,   sizeof("--- 00"),   "%b %d"    , tick_time);
  
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
  
  static char sdeltat[10];
  long deltat = (weather_buffer.time - temp)/60;
  if(deltat > 10){
    snprintf(sdeltat, sizeof(sdeltat), "DT:!%li!", deltat);
  }
  else{
    snprintf(sdeltat, sizeof(sdeltat), "DT:%li", deltat);
  }
  set_text_update_time(sdeltat);

}

static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static GFont s_res_bitham_30_black;


static void initialise_ui(void) {
  s_window = window_create();
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
  month_layer = text_layer_create(GRect(90, 14, 51, 30));
  text_layer_set_background_color(month_layer, GColorClear);
  text_layer_set_text_color(month_layer, GColorWhite);
  text_layer_set_text(month_layer, "apr 23");
  text_layer_set_text_alignment(month_layer, GTextAlignmentRight);
  text_layer_set_font(month_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)month_layer);
  
  // temperature_layer
  temperature_layer = text_layer_create(GRect(88, 90, 52, 39));
  text_layer_set_text(temperature_layer, "15°C");
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  text_layer_set_font(temperature_layer, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)temperature_layer);
  
  // condition_layer
  condition_layer = text_layer_create(GRect(4, 90, 80, 39));
  text_layer_set_text(condition_layer, "Scattered Clouds");
  text_layer_set_text_alignment(condition_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)condition_layer);
  
  // year_layer
  year_layer = text_layer_create(GRect(4, 14, 42, 28));
  text_layer_set_background_color(year_layer, GColorClear);
  text_layer_set_text_color(year_layer, GColorWhite);
  text_layer_set_text(year_layer, "2015");
  text_layer_set_text_alignment(year_layer, GTextAlignmentCenter);
  text_layer_set_font(year_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)year_layer);
  
  // location_layer
  location_layer = text_layer_create(GRect(4, 144, 94, 20));
  text_layer_set_text(location_layer, "Hampsted");
  text_layer_set_text_alignment(location_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)location_layer);
  
  // update_time_layer
  update_time_layer = text_layer_create(GRect(100, 144, 40, 20));
  text_layer_set_text(update_time_layer, "DT:10");
  text_layer_set_text_alignment(update_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)update_time_layer);
  
  // weekday_layer
  weekday_layer = text_layer_create(GRect(54, 14, 33, 30));
  text_layer_set_background_color(weekday_layer, GColorClear);
  text_layer_set_text_color(weekday_layer, GColorWhite);
  text_layer_set_text(weekday_layer, "Wed");
  text_layer_set_text_alignment(weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(weekday_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)weekday_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(title_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(month_layer);
  text_layer_destroy(temperature_layer);
  text_layer_destroy(weekday_layer);
  text_layer_destroy(condition_layer);
  text_layer_destroy(year_layer);
  text_layer_destroy(location_layer);
  text_layer_destroy(update_time_layer);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void set_text_title(char* title){
  text_layer_set_text(title_layer, title);
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
static void set_text_update_time(char* update_time){
  text_layer_set_text(update_time_layer, update_time);
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