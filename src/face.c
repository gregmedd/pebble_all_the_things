#include <pebble.h>
#include "countries.h"

static Window *watch_window;
static TextLayer *time_layer;
static BitmapLayer *map_layer;
static BitmapLayer *heart_layer;
#ifdef PBL_BW
  static TextLayer *equal_layer;
#endif

static GBitmap *map_img;
static GBitmap *heart_img;

static char time_buffer[6];

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (units_changed & (MINUTE_UNIT | HOUR_UNIT)) {
    if (clock_is_24h_style()) {
      strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
    } else {
      strftime(time_buffer, sizeof(time_buffer), "%I:%M", tick_time);
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting the time to %s", time_buffer);
    text_layer_set_text(time_layer, time_buffer);
  }
  
  if (units_changed & SECOND_UNIT) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Make the heart beat");
    layer_set_hidden((Layer *)heart_layer,
                     ((tick_time->tm_sec % 2) == 1) || !bluetooth_connection_service_peek());
    #ifdef PBL_BW
      text_layer_set_text_color(equal_layer,
                               ((tick_time->tm_sec % 2) == 1) || !bluetooth_connection_service_peek()
                               ? GColorWhite : GColorBlack);
    #endif
  }
}

void watch_window_load(Window *window) {
  // Set up the text layer for the digital time
  time_layer = text_layer_create(GRect(0, 10, 144, 48));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  
  // Set up the equal sign layer for the B&W version
  #ifdef PBL_BW
    equal_layer = text_layer_create(GRect(0, 83, 144, 48));
    text_layer_set_text_color(equal_layer, GColorBlack);
    text_layer_set_background_color(equal_layer, GColorClear);
    text_layer_set_font(equal_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(equal_layer, GTextAlignmentCenter);
    text_layer_set_text(equal_layer, "=");
  #endif

  // Set up layer for the map graphic
  map_layer = bitmap_layer_create(GET_COUNTRY_GRECT(DefaultCountry));
  map_img = gbitmap_create_with_resource(CountryMaps[DefaultCountry]);
  bitmap_layer_set_compositing_mode(map_layer, GCompOpSet);
  bitmap_layer_set_bitmap(map_layer, map_img);
  
  // And the heart graphic
  heart_layer = bitmap_layer_create(GRect(0, 68, 144, 90));
  heart_img = gbitmap_create_with_resource(RESOURCE_ID_BT_HEART);
  bitmap_layer_set_compositing_mode(heart_layer, GCompOpSet);
  bitmap_layer_set_bitmap(heart_layer, heart_img);

  // Put together the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(map_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(heart_layer));
  #ifdef PBL_BW
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(equal_layer));
  #endif
  window_set_background_color(window, GColorBlack);
  
  // Should probably do something here to make the time get set instead of waiting
  time_t init_time = time(NULL);
  tick_handler(localtime(&init_time), SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT);
  
  // Finally, register for timer updates
  tick_timer_service_subscribe(SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT, tick_handler);
}

void watch_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  text_layer_destroy(time_layer);
  bitmap_layer_destroy(map_layer);
  bitmap_layer_destroy(heart_layer);
  gbitmap_destroy(map_img);
  gbitmap_destroy(heart_img);
  #ifdef PBL_BW
    text_layer_destroy(equal_layer);
  #endif
}

void handle_init(void) {
  // Set up the main watch window
  watch_window = window_create();
  window_set_background_color(watch_window, GColorBlack);
  window_set_window_handlers(watch_window, (WindowHandlers) {
    .load = watch_window_load,
    .unload = watch_window_unload
  });

  window_stack_push(watch_window, true);
}

void handle_deinit(void) {
  window_destroy(watch_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}