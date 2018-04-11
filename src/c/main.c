#include <pebble.h>
#include "main.h"

//declaring pointers
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static BitmapLayer *s_backgroundBlink_layer;
static GBitmap *s_backgroundBlink_bitmap;

static void update_time()
  {
  //get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  //create a long-lived buffer
  static char buffer[]= "00:00";
  
  //write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true)
    {
    //use 24 houor format
    strftime (buffer, sizeof("00:00"), "%H:%M", tick_time);
    }
  else
    {
    //use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
  //display the time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
     
}

static void main_window_load(Window *window)
  {
  //create GBitmap, then set to create BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_image_shadowCat_identifier);
  s_background_layer = bitmap_layer_create(GRect(-5,-10,144,155));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  //create a GBitmap as a second layer ontop of the above for the blink image
  s_backgroundBlink_bitmap = gbitmap_create_with_resource(RESOURCE_ID_image_ShadowCat_blink_identifier);
  s_backgroundBlink_layer = bitmap_layer_create(GRect(-5,18,144,15));
  bitmap_layer_set_bitmap(s_backgroundBlink_layer, s_backgroundBlink_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_backgroundBlink_layer));
  
  //make the bitmap for the blink invisible
  layer_set_hidden(bitmap_layer_get_layer(s_backgroundBlink_layer), true);
  
  //create time layer
  s_time_layer = text_layer_create(GRect(2, 140, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  //create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font_arial_24));
  //apply to textLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  //add it as a child layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window)
  {
  //destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_backgroundBlink_bitmap);
  //destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_backgroundBlink_layer);
  //Destroy GFont
  fonts_unload_custom_font(s_time_font);
  //destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void blink_handler(void *catty)
  {
  if(layer_get_hidden(bitmap_layer_get_layer(s_backgroundBlink_layer)))
    {
  //make the layer invisible   
  layer_set_hidden(bitmap_layer_get_layer(s_backgroundBlink_layer), false);
    app_timer_register(300, blink_handler, NULL);
    }
  else
    {
  //make layer visible
  layer_set_hidden(bitmap_layer_get_layer(s_backgroundBlink_layer), true);
    app_timer_register(6000, blink_handler, NULL);
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
  {
  update_time();
}

static void init()
  {
  //create main window element and assign to pointer
  s_main_window = window_create();
  //set handlers to manage the elements inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers)
                            {
                              .load = main_window_load,
                              .unload = main_window_unload
                            });
  //show the window on the watch, with animated = true
  window_stack_push(s_main_window, true);
  //make sure the time is displayed from the start
  update_time();
  //register with the TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  //register the blink with the TickTimerService
  app_timer_register(5000, blink_handler, NULL);
}

static void deinit()
{
  //destroy window
  window_destroy(s_main_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}