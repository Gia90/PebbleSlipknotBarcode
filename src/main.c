#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_steps_layer, *s_battery_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static GFont s_global_font;

static int s_step_count = 0, s_battery_level;

/** TIME & DATE **/
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  snprintf(s_buffer, sizeof(s_buffer), "%d %d %d %d", tick_time->tm_hour/10, (tick_time->tm_hour)%10, tick_time->tm_min/10, tick_time->tm_min%10);
  //strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H %M" : "%I %M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  // Write the current date into a buffer
  static char date_buffer[16];
  //strftime(date_buffer, sizeof(date_buffer), "%d %m %y", tick_time);
  snprintf(date_buffer, sizeof(date_buffer), "%d %d %d %d %d %d", (tick_time->tm_mday)/10, (tick_time->tm_mday)%10, (tick_time->tm_mon + 1)/10, (tick_time->tm_mon + 1)%10, (tick_time->tm_year + 1900)%100/10, (tick_time->tm_year + 1900)%100%10);
  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
/** END TIME & DATE **/

/** HEALTH **/
// Is step data available?
bool step_data_is_available() {
  return HealthServiceAccessibilityMaskAvailable & health_service_metric_accessible(HealthMetricStepCount, time_start_of_today(), time(NULL));
}

static void update_steps() {
  // Write the current steps into a buffer
  static char steps_buffer[3];
  snprintf(steps_buffer, sizeof(steps_buffer), "%d", s_step_count/1000);
  // Display steps on the TextLayer
  text_layer_set_text(s_steps_layer, steps_buffer);
}

static void health_handler(HealthEventType event, void *context) {
  if(event != HealthEventSleepUpdate) {
    s_step_count = (int) health_service_sum_today(HealthMetricStepCount);
    update_steps();
  }
}
/** END HEALTH **/

/** BATTERY **/
static void update_battery() {
  // Write the current battery into a buffer
  static char battery_buffer[2];
  snprintf(battery_buffer, sizeof(battery_buffer), "%c", (s_battery_level > 90) ? 'F' : (s_battery_level/10+48) );
  // Display battery on the TextLayer
  text_layer_set_text(s_battery_layer, battery_buffer);
}

static void battery_handler(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  update_battery();
}
/** END BATTERY **/

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  // Create custom GFont
  s_global_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FAKEPLASTIC_36));
  // Use system Font
  //s_global_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);

  // Create the TextLayer with specific bounds
  //s_time_layer = text_layer_create(GRect(0, 118, 83, 50));
  s_time_layer = text_layer_create(GRect(0, 120, 79, 50));
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "6 6 6 6");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_global_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // DATE
  // Create date TextLayer
  //s_date_layer = text_layer_create(GRect(0, 118, 197, 50));
  s_date_layer = text_layer_create(GRect(0, 120, 195, 50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "6 6 6 6 6 6");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_date_layer, s_global_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  // HEALTH
  // Create steps TextLayer
  //s_steps_layer = text_layer_create(GRect(0, 125, 26, 50));
  s_steps_layer = text_layer_create(GRect(0, 127, 21, 50));
  text_layer_set_background_color(s_steps_layer, GColorClear);
  text_layer_set_text_color(s_steps_layer, GColorBlack);
  text_layer_set_text(s_steps_layer, "6");
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_steps_layer, s_global_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_steps_layer));
  
  // BATTERY
  // Create battery TextLayer (0, 145, 270, 50)
  //s_battery_layer = text_layer_create(GRect(0, 125, 270, 50));
  s_battery_layer = text_layer_create(GRect(0, 127, 270, 50));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  text_layer_set_text(s_battery_layer, "6");
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_battery_layer, s_global_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(s_battery_layer);
  
  // Unload Custom Font (ONLY FOR CUSTOM FONT!)
  fonts_unload_custom_font(s_global_font);

  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Make sure the time is displayed from the start
  update_time();
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_handler);
  // Ensure battery level is displayed from the start
  battery_handler(battery_state_service_peek());
  
  // Subscribe to health events if we can
  if(step_data_is_available()) {
    health_service_events_subscribe(health_handler, NULL);
  }
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}