#include <pebble.h>
#include <math.h>
#include "main.h"
#include "path_definitions.c"

#define ROUND PBL_IF_ROUND_ELSE(true, false)
#define COLORS PBL_IF_COLOR_ELSE(true, false)

static Window *s_main_window;
static TextLayer *s_time_layer;
static Layer *s_analog_hands_layer;

// Path based watchface defs
static Layer *s_face_layer;

// Color defs for path watchface
static GColor *s_primary_color; // Used for markings on OV, and default status color on Classic

// Watch status
static bool s_bluetooth_connected;
static int s_battery_level;

// Var that decides if time should be showing or not.
// Ignored if settings.only_show_time_on_shake is false
static bool s_should_show_time;

static int w;
static int h;

// A struct for our specific settings (see main.h)
ClaySettings settings;

static GColor update_status_light() {
  //Battery and connection status light

    if (!s_bluetooth_connected) {
      // Show bluetooth disconnected light
      text_layer_set_text_color(s_time_layer, GColorBlack);
      if (COLORS) {
        return GColorChromeYellow;
      } else {
        return GColorDarkGray;
      }
    } else if (s_battery_level <= 0.1) {
      // Show low battery light
      text_layer_set_text_color(s_time_layer, GColorBlack);
      if (COLORS) {
        return GColorRed;
      } else {
        return GColorDarkGray;
      }
    } else {
      // No problems here, use default light and text colors
      if (settings.classic_dial_style) {
        text_layer_set_text_color(s_time_layer, GColorBlack);
        if (COLORS) {
          return GColorSpringBud;
        } else {
          return GColorWhite;
        }
      } else {
        text_layer_set_text_color(s_time_layer, GColorWhite);
        return GColorBlack;
      }
    }
  
}

// Initialize the default settings
static void prv_default_settings() {
  settings.use_analog_time = false;
  settings.classic_dial_style = false;
  settings.only_show_time_on_shake = false;
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

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// Update the display elements
static void prv_update_display() {
  // Reset tick timer service
  tick_timer_service_unsubscribe();
  
  // Check if time should be showing. This should do:
  // If the settings switch is turned off, always show time.
  // If the settings switch is turned on and time should be showing, show time.
  if (!settings.only_show_time_on_shake || (settings.only_show_time_on_shake && s_should_show_time)) {
    // Time should be showing, resub to tick timer
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
    // Analog or digital
    if (settings.use_analog_time) {
      // Show analog layer, hide digital layer
      layer_set_hidden(s_analog_hands_layer, false);
      layer_set_hidden(text_layer_get_layer(s_time_layer), true);
    } else {
      // Show digital layer, hide analog layer
      layer_set_hidden(s_analog_hands_layer, true);
      layer_set_hidden(text_layer_get_layer(s_time_layer), false);
    }
  } else {
    // Hide both layers
      layer_set_hidden(s_analog_hands_layer, true);
      layer_set_hidden(text_layer_get_layer(s_time_layer), true);
  }

  layer_mark_dirty(s_face_layer);
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

  // only show time when shaken
  Tuple *only_show_shaken_t = dict_find(iter, MESSAGE_KEY_WatchDialStyle);
  if (only_show_shaken_t) {
    settings.only_show_time_on_shake = only_show_shaken_t->value;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

static void bluetooth_callback(bool connected) {
  s_bluetooth_connected = connected;
  
  update_status_light();
  
  if(!connected) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void battery_callback(BatteryChargeState state) {
  // Record new battery level
  s_battery_level = (int) state.charge_percent;
   update_status_light();
}

static void on_timer_complete() {
  s_should_show_time = false;
  prv_update_display();
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // A tap event occured
  if (settings.only_show_time_on_shake) {
    s_should_show_time = true;

    AppTimer *s_show_time_timer = app_timer_register(5000, on_timer_complete, NULL);

    prv_update_display();
  } else {
    return;
  }

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

static void face_layer_update_proc(Layer *layer, GContext *ctx) {
  // Figure out what color to draw status light

  if (settings.classic_dial_style) {
    //figure out if round or not
    if (ROUND) {
      // draw classic/round
      graphics_context_set_fill_color(ctx, GColorDarkGray);
      graphics_fill_rect(ctx, GRect(0, 0, 180, 180), 0, GCornersAll);

      // Status light
      graphics_context_set_fill_color(ctx, update_status_light());
      gpath_draw_filled(ctx, gpath_create(&PATH_CLASSIC_STATUS_LIGHT_ROUND));

      // Black carets
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, gpath_create(&PATH_CLASSIC_LEFT_CARET_ROUND));
      gpath_draw_filled(ctx, gpath_create(&PATH_CLASSIC_RIGHT_CARET_ROUND));
    } else{
      // draw classic/rect
      graphics_context_set_fill_color(ctx, GColorDarkGray);
      graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornersAll);

      // Status light
      graphics_context_set_fill_color(ctx, update_status_light());
      gpath_draw_filled(ctx, gpath_create(&PATH_CLASSIC_STATUS_LIGHT));

      // Black carets
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, gpath_create(&PATH_CLASSIC_LEFT_CARET));
      gpath_draw_filled(ctx, gpath_create(&PATH_CLASSIC_RIGHT_CARET));

      // Circle stroke
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_stroke_width(ctx, 12);
      graphics_draw_circle(ctx, GPoint(72, 84), 68);
    }
  } else {
    // draw the OV face
    
    //figure out if round or not
    if (ROUND) {
      // draw OV/round
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_rect(ctx, GRect(0, 0, 180, 180), 0, GCornersAll);

      // Circle black bg
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_fill_circle(ctx, GPoint(90, 90), 72);

      // Status light
      graphics_context_set_fill_color(ctx, update_status_light());
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_STATUS_LIGHT_ROUND));

      // Circle stroke overlay
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_stroke_width(ctx, 4);
      graphics_draw_circle(ctx, GPoint(90, 90), 70);
      
      // Marking BG/stroke
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_LEFT_MARKING_BG_ROUND));
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_RIGHT_MARKING_BG_ROUND));

      // Markings
      graphics_context_set_fill_color(ctx, GColorJaegerGreen);
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_LEFT_MARKING_ROUND));
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_RIGHT_MARKING_ROUND));

    } else{
      // draw OV/rect
      // Status light/center area
      graphics_context_set_fill_color(ctx, update_status_light());
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_STATUS_LIGHT));

      // black BG and stroke behind markings
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_LEFT_MARKING_BG));
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_RIGHT_MARKING_BG));
  
      // Markings
      if (COLORS) {
        graphics_context_set_fill_color(ctx, GColorJaegerGreen);
      } else {
        graphics_context_set_fill_color(ctx, GColorWhite);
      }
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_LEFT_MARKING));
      gpath_draw_filled(ctx, gpath_create(&PATH_OV_RIGHT_MARKING));
    }
  }
  
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  w = bounds.size.w;
  h = bounds.size.h;

  // Create layer for path-based face
  s_face_layer = layer_create(bounds);
  layer_set_update_proc(s_face_layer, face_layer_update_proc);
  layer_add_child(window_layer, s_face_layer);  
  
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
  layer_destroy(s_face_layer);
  layer_destroy(s_analog_hands_layer);
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

  // Subscribe to tap events
  accel_tap_service_subscribe(accel_tap_handler);
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
  .pebble_app_connection_handler = bluetooth_callback});
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  app_message_deregister_callbacks();
  accel_tap_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}