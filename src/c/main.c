#include <pebble.h>
#include <math.h>
#include "main.h"

#define ROUND PBL_IF_ROUND_ELSE(true, false)
#define COLORS PBL_IF_COLOR_ELSE(true, false)

static Window *s_main_window;
static BitmapLayer *s_face_layer;
static GBitmap *s_omniverse_bitmap;
static GBitmap *s_classic_bitmap;
static TextLayer *s_time_layer;
static Layer *s_analog_hands_layer;

static bool s_bluetooth_connected;
static int s_battery_level;

static int w;
static int h;

// A struct for our specific settings (see main.h)
ClaySettings settings;

static void update_status_light() {
  //Battery and connection status light
  if (COLORS) {
    if (!s_bluetooth_connected) {
      // Show bluetooth disconnected light
      bitmap_layer_set_background_color(s_face_layer, GColorChromeYellow);
      text_layer_set_text_color(s_time_layer, GColorBlack);
    } else if (s_battery_level <= 0.1) {
      // Show low battery light
      bitmap_layer_set_background_color(s_face_layer, GColorRed);
      text_layer_set_text_color(s_time_layer, GColorBlack);
    } else {
      // No problems here, use default light and text colors
      if (settings.classic_dial_style) {
        bitmap_layer_set_background_color(s_face_layer, GColorBrightGreen);
        text_layer_set_text_color(s_time_layer, GColorBlack);
      } else {
        bitmap_layer_set_background_color(s_face_layer, GColorBlack);
        text_layer_set_text_color(s_time_layer, GColorWhite);
      }
    }
  } else {
    // Device doesnt support colors, use default text color
      if (settings.classic_dial_style) {
        text_layer_set_text_color(s_time_layer, GColorBlack);
      } else {
        text_layer_set_text_color(s_time_layer, GColorWhite);
      }
  }
}

// Initialize the default settings
static void prv_default_settings() {
  settings.use_analog_time = false;
  settings.classic_dial_style = false;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
  // Analog or digital
  if (settings.use_analog_time) {
    layer_set_hidden(s_analog_hands_layer, false);
    layer_set_hidden(text_layer_get_layer(s_time_layer), true);
  } else {
    layer_set_hidden(s_analog_hands_layer, true);
    layer_set_hidden(text_layer_get_layer(s_time_layer), false);
  }
  
  // Classic or OV
  if (settings.classic_dial_style) {
    bitmap_layer_set_bitmap(s_face_layer, s_classic_bitmap);
  } else {
    bitmap_layer_set_bitmap(s_face_layer, s_omniverse_bitmap);
  }
  
  update_status_light();
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Analog time
  Tuple *time_display_mode_t = dict_find(iter, MESSAGE_KEY_TimeDisplayMode);
  if (time_display_mode_t) {
    settings.use_analog_time = strcmp(time_display_mode_t->value->cstring, "analog") == 0;
  }
  
  // Omnitrix style
  Tuple *watch_dial_style_t = dict_find(iter, MESSAGE_KEY_WatchDialStyle);
  if (watch_dial_style_t) {
    settings.classic_dial_style = strcmp(watch_dial_style_t->value->cstring, "classic") == 0;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

static void bluetooth_callback(bool connected) {
  s_bluetooth_connected = connected;
  
  if (COLORS) {
    update_status_light();
  }
  
  if(!connected) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void battery_callback(BatteryChargeState state) {
  // Record new battery level
  s_battery_level = (int) state.charge_percent;
   if (COLORS) {
     update_status_light();
   }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H\n%M" : "%I\n%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  //Update the analog layer
  layer_mark_dirty(s_analog_hands_layer);
}

int getTenthOf(float value, float tenth) {
  return (int)value/(10/tenth);
}

static void analog_hands_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  const int16_t max_hand_length = (bounds.size.w / 2) - 19;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  // calculate minute hand
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)max_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)max_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  // draw minute hand
  // Figure out what color to draw
  if (settings.classic_dial_style) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  } else {
    graphics_context_set_stroke_color(ctx, GColorBlack);
  }
  graphics_context_set_stroke_width(ctx, 12);
  graphics_draw_line(ctx, minute_hand, center);
  // Figure out what color to draw
  if (settings.classic_dial_style) {
    graphics_context_set_stroke_color(ctx, GColorBlack);
  } else {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  }
  graphics_context_set_stroke_width(ctx, 8);
  graphics_draw_line(ctx, minute_hand, center);
  
  // calculate hour hand
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)(max_hand_length * 0.6) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)(max_hand_length * 0.6) / TRIG_MAX_RATIO) + center.y,
  };
  // draw hour hand
  // Figure out what color to draw
  if (settings.classic_dial_style) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  } else {
    graphics_context_set_stroke_color(ctx, GColorBlack);
  }
  graphics_context_set_stroke_width(ctx, 12);
  graphics_draw_line(ctx, hour_hand, center);
  // Figure out what color to draw
  if (settings.classic_dial_style) {
    graphics_context_set_stroke_color(ctx, GColorBlack);
  } else {
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorGreen, GColorWhite));
  }
  graphics_context_set_stroke_width(ctx, 8);
  graphics_draw_line(ctx, hour_hand, center);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  w = bounds.size.w;
  h = bounds.size.h;
  
  // Create the layer that shows the Omnitrix dial
  s_face_layer = bitmap_layer_create(bounds);
  
  // Set the bitmap onto the layer and add to the window
  // Create GBitmap
  s_omniverse_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OMNIVERSE_BG);
  s_classic_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLASSIC_BG);
  //bitmap_layer_set_bitmap(s_face_layer, s_omniverse_bitmap);
  bitmap_layer_set_compositing_mode(s_face_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_face_layer));
  
  
  // Create analog hands Layer
  s_analog_hands_layer = layer_create(bounds);
  
  // Set this Layer's update procedure
  layer_set_update_proc(s_analog_hands_layer, analog_hands_layer_update_proc); 
  
  layer_add_child(window_layer, s_analog_hands_layer);
  
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(0, (h/2)-47, bounds.size.w, 90));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  update_time();
  prv_update_display();
  update_status_light();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  // Destroy face layer
  bitmap_layer_destroy(s_face_layer);
  layer_destroy(s_analog_hands_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_omniverse_bitmap);
  gbitmap_destroy(s_classic_bitmap);
}

static void init() {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the battery has an initial value
  s_battery_level = (int) battery_state_service_peek().charge_percent;
  
  // Show the correct state of the BT connection from the start
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
  .pebble_app_connection_handler = bluetooth_callback});
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  app_message_deregister_callbacks();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}