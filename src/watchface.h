#pragma once
#include <pebble.h>
// https://github.com/clach04/watchface_framework

Window *s_main_window;
TextLayer *s_time_layer, *s_date_layer, *s_steps_layer, *s_battery_layer;

BitmapLayer *s_background_layer;
GBitmap *s_background_bitmap;

GFont s_global_font;
GColor s_global_font_color;

// CONFIGURATION VARS (PRIVATE)
bool config_invert;

typedef enum {DMY, MDY, YMD} date_format_t;
date_format_t config_date_format;



/******************* FEATURES FUNCTIONS **********************/

/** TIME & DATE **/
extern void update_time();
extern void tick_handler(struct tm *tick_time, TimeUnits units_changed);
/** END TIME & DATE **/

/** HEALTH **/
// Is step data available?
extern bool step_data_is_available();
extern void update_steps(int steps);
extern void health_handler(HealthEventType event, void *context);
/** END HEALTH **/

/** BATTERY **/
extern void update_battery(int battery_level);
extern void battery_handler(BatteryChargeState state);
/** END BATTERY **/


/**************** CONFIGURATION FUNCTIONS ********************/
// Handle new config inbox message received
extern void inbox_received_handler(DictionaryIterator *iterator, void *context);

// Load existing config from persistent storage
extern void load_config();
// Update element colors and, optionally, refresh the ui
extern void apply_config_invert(bool update_ui);