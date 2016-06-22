#include <pebble.h>
#include "watchface.h"

static void main_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load!");
  
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Apply invert config without refreshing the ui (which has not been created yet)
  apply_config_invert(false);
  
  // Create GBitmap
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  // Create custom GFont
  s_global_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FAKEPLASTIC_36));

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, 120, 79, 50));
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, s_global_font_color);
  text_layer_set_text(s_time_layer, "6 6 6 6");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_global_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  // DATE
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 120, 195, 50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, s_global_font_color);
  text_layer_set_text(s_date_layer, "6 6 6 6 6 6");
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_date_layer, s_global_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  // HEALTH
  // Create steps TextLayer
  s_steps_layer = text_layer_create(GRect(0, 127, 21, 50));
  text_layer_set_background_color(s_steps_layer, GColorClear);
  text_layer_set_text_color(s_steps_layer, s_global_font_color);
  text_layer_set_text(s_steps_layer, "6");
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_steps_layer, s_global_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_steps_layer));
  
  // BATTERY
  // Create battery TextLayer (0, 145, 270, 50)
  s_battery_layer = text_layer_create(GRect(0, 127, 270, 50));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, s_global_font_color);
  text_layer_set_text(s_battery_layer, "6");
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  // Apply to TextLayer
  text_layer_set_font(s_battery_layer, s_global_font);
  // Add to Window
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
}

static void main_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_UNload!");
  
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
  // Load config from persistent storage
  load_config();
  
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
  
  // Config handler
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
  //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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