#include <pebble.h>
#include "watchface.h"

Window *s_main_window;
TextLayer *s_time_layer, *s_date_layer, *s_steps_layer, *s_battery_layer;

BitmapLayer *s_background_layer;
GBitmap *s_background_bitmap;

GFont s_global_font;
GColor s_global_font_color;


// CONFIGURATION VARS
bool config_invert = false;
date_format_t config_date_format = DMY;


/***************************** FEATURES FUNCTIONS **********************************/

/** TIME & DATE **/
void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  snprintf(s_buffer, sizeof(s_buffer), "%d %d %d %d", tick_time->tm_hour/10, (tick_time->tm_hour)%10, tick_time->tm_min/10, tick_time->tm_min%10);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  // Write the current date into a buffer
  static char date_buffer[16];
  int d[2] = { (tick_time->tm_mday)/10, (tick_time->tm_mday)%10 },
      m[2] = { (tick_time->tm_mon + 1)/10, (tick_time->tm_mon + 1)%10 },
      y[2] = { (tick_time->tm_year + 1900)%100/10, (tick_time->tm_year + 1900)%100%10 };
  
  switch(config_date_format) {
   case MDY:
      snprintf(date_buffer, sizeof(date_buffer), "%d %d %d %d %d %d", m[0], m[1], d[0], d[1], y[0], y[1]);
      break;
	 case YMD:
      snprintf(date_buffer, sizeof(date_buffer), "%d %d %d %d %d %d", y[0], y[1], m[0], m[1], d[0], d[1]);
      break;
   default : /* DMY */
      snprintf(date_buffer, sizeof(date_buffer), "%d %d %d %d %d %d",  d[0], d[1], m[0], m[1], y[0], y[1]); 
  }
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
/** END TIME & DATE **/

/** HEALTH **/
// Is step data available?
bool step_data_is_available() {
  return HealthServiceAccessibilityMaskAvailable & health_service_metric_accessible(HealthMetricStepCount, time_start_of_today(), time(NULL));
}

void update_steps(int steps) {
  // Write the current steps into a buffer
  static char steps_buffer[3];
  snprintf(steps_buffer, sizeof(steps_buffer), "%d", steps/1000);
  // Display steps on the TextLayer
  text_layer_set_text(s_steps_layer, steps_buffer);
}

void health_handler(HealthEventType event, void *context) {
  if(event != HealthEventSleepUpdate) {
    update_steps((int) health_service_sum_today(HealthMetricStepCount));
  }
}
/** END HEALTH **/

/** BATTERY **/
void update_battery(int battery_level) {
  // Write the current battery into a buffer
  static char battery_buffer[2];
  snprintf(battery_buffer, sizeof(battery_buffer), "%c", (battery_level > 90) ? 'F' : (battery_level/10+48) );
  // Display battery on the TextLayer
  text_layer_set_text(s_battery_layer, battery_buffer);
}

void battery_handler(BatteryChargeState state) {
  // Record the new battery level
  update_battery(state.charge_percent);
}
/** END BATTERY **/


/**************************** FEATURES FUNCTIONS *********************************/
// Handle new config inbox message received
void inbox_received_handler(DictionaryIterator *iterator, void *context)
{
  Tuple *t=NULL;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox_received_handler() called");
  
  // Config received for inverted colors
  t = dict_find(iterator, MESSAGE_KEY_INVERT);

  if (t) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_INVERT");
    config_invert = (bool)t->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "Persisting config_invert: %d", (int) config_invert);
    persist_write_bool(MESSAGE_KEY_INVERT, config_invert);
  
    // Reload Watchface!
    apply_config_invert(true);
  }
  
  // Config received for new date format
  t = dict_find(iterator, MESSAGE_KEY_DATE_FORMAT);

  if (t) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "got MESSAGE_KEY_DATE_FORMAT");
    
    config_date_format = atoi(t->value->cstring);
    
    APP_LOG(APP_LOG_LEVEL_INFO, "Received date_format: %s", t->value->cstring);
    APP_LOG(APP_LOG_LEVEL_INFO, "Persisting date_format: %d", config_date_format);
    
    persist_write_int(MESSAGE_KEY_DATE_FORMAT, config_date_format);
    
    // Reload Watchface!
    update_time();
  }
}

// Load existing config from persistent storage
void load_config() {
  if (persist_exists(MESSAGE_KEY_INVERT)) {
    config_invert = persist_read_bool(MESSAGE_KEY_INVERT);
  }
  if (persist_exists(MESSAGE_KEY_DATE_FORMAT)) {
    config_date_format = persist_read_int(MESSAGE_KEY_DATE_FORMAT);
  }
}

// Update element colors and, optionally, refresh the ui
void apply_config_invert(bool update_ui) {
  if(s_background_bitmap != NULL)
    gbitmap_destroy(s_background_bitmap);
  
  if(config_invert) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_INVERTED);
    s_global_font_color = GColorWhite;
  }
  else {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    s_global_font_color = GColorBlack;
  }
  
  if(update_ui) {
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

    text_layer_set_text_color(s_time_layer, s_global_font_color);
    text_layer_set_text_color(s_date_layer, s_global_font_color);
    text_layer_set_text_color(s_steps_layer, s_global_font_color);
    text_layer_set_text_color(s_battery_layer, s_global_font_color);
  }
}






