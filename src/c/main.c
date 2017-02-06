#include <pebble.h>

#define SETTINGS_KEY 1

// Initially based on LowPoly by ECandB - big thanks and shout out!! //

static Window *main_window;
static TextLayer *time_layer, *day_layer, *last_layer, *bt_layer, *date_layer, *step_layer, *percent_layer, *xpercent_layer, *datecolour_layer, *stepcolour_layer, *timecolour_layer;
// static GBitmap *bg_bitmap, *bt_icon_bitmap, *battery_icon_bitmap;
//   , *charging_icon_bitmap;
// static BitmapLayer *bg_bitmap_layer, *bt_icon_layer, *battery_icon_layer;
static GFont timeFont, dateFont, percentFont;
static Layer *battery_layer;
static int battery_level; 
// static bool battery_plugged;

typedef enum {
  AppKeyBackgroundColor = 0,
  AppKeyVibrate = 1,
  AppKeyLeadingZero = 2,
  AppKeyShowBattery = 3
} AppKey;

// A structure containing our settings
typedef struct ClaySettings {
  int StepGoal;
  bool buzzondisconnect;
  int timecolour;
  int datecolour;
  int timetextcolour;
  int datetextcolour;
  int facecolour;
} __attribute__((__packed__)) ClaySettings;

// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.StepGoal = 4000;
  settings.buzzondisconnect = 0;
  settings.timecolour = 0x0000FF;
  settings.datecolour = 0x0055FF;
  settings.timetextcolour = 0xFFFFFF;
  settings.datetextcolour = 0xFFFFFF;
  settings.facecolour = 0x000000;
  
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
  // update_time();
}

static void health_handler(HealthEventType event, void *context) {
  static char s_value_buffer[8];
  
  if (event == HealthEventMovementUpdate || event == HealthEventSignificantUpdate) {
    // display the step count
    snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", (int)health_service_sum_today(HealthMetricStepCount));
    text_layer_set_text(step_layer, s_value_buffer);
    
    int step_count = atoi(s_value_buffer);
        
    if (settings.StepGoal > 0) {
       if(step_count > settings.StepGoal) {
          text_layer_set_background_color(stepcolour_layer, GColorGreen);
          } 
       else if(step_count < 2000) {
          text_layer_set_background_color(stepcolour_layer, GColorRed);
          } 
       else {
          text_layer_set_background_color(stepcolour_layer, GColorBlack);
            }
        }
    }
}

static void battery_callback(BatteryChargeState state) {
  battery_level = state.charge_percent;
  // battery_plugged = state.is_charging;
  
  static char s_percent_buffer[5];
  
  snprintf(s_percent_buffer, sizeof(s_percent_buffer), "%d%%", (int)battery_level);
    
  text_layer_set_text(percent_layer, s_percent_buffer);
  
  if(battery_level > 20 ) {
       text_layer_set_background_color(xpercent_layer, GColorBlack);
       // text_layer_set_background_color(percent_layer, GColorBlack);
    };  

    if(battery_level < 21) {
       text_layer_set_background_color(xpercent_layer, GColorRed);
       // text_layer_set_background_color(percent_layer, GColorRed);
    }; 
  
  layer_mark_dirty(battery_layer);
}


// static void battery_update_proc(Layer *layer, GContext *ctx) {
//  GRect bounds = layer_get_bounds(layer);
//  int width = (int)(float)(((float)battery_level / 100.0F) * 16.0F);

  // Draw the background
 // if(battery_level < 21){
 //    graphics_context_set_fill_color(ctx, GColorRed);       
 // } else {
 //    graphics_context_set_fill_color(ctx, GColorBlack);   
 // }

//  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
//  if(battery_level < 21){
//     graphics_context_set_fill_color(ctx, GColorLightGray); // was red
//  } else {
//    graphics_context_set_fill_color(ctx, GColorWhite);
//  }
//  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);

// }

static void bluetooth_callback(bool connected) {
  //layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), connected);
  // layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), true);
 
  if(connected) 
  {
     text_layer_set_text(bt_layer, "");
     text_layer_set_background_color(bt_layer, GColorBlack);
  };    
  
  if(!connected) 
  {
     text_layer_set_text(bt_layer, "bt!");
     text_layer_set_background_color(bt_layer, GColorRed);
     if (settings.buzzondisconnect) {
        if (!quiet_time_is_active()) {
           vibes_double_pulse();
        }
      }
  };
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Time
  static char time_buffer[8];
  strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(time_layer, time_buffer);

  // Date
  static char date_buffer[32];
  strftime(date_buffer, sizeof(date_buffer), "%d %b", tick_time);
  // upcase(date_buffer);
  text_layer_set_text(date_layer, date_buffer);
  
  static char day_buffer[32];

  strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);
  
  if (strcmp(day_buffer, "Wednesday") == 0) 
  {
    strcpy(day_buffer, "Weds.");
  };
  if (strcmp(day_buffer, "Thursday") == 0) 
  {
    strcpy(day_buffer, "Thurs.");
  };
  if (strcmp(day_buffer, "Saturday") == 0) 
  {
    strcpy(day_buffer, "Sat.");
  };  
  // upcase(date_buffer);
  text_layer_set_text(day_layer, day_buffer);

  // Colour stuff here
  
  window_set_background_color(main_window, GColorFromHEX(settings.facecolour));
  text_layer_set_background_color(timecolour_layer, GColorFromHEX(settings.timecolour));
  text_layer_set_background_color(datecolour_layer, GColorFromHEX(settings.datecolour));
  text_layer_set_text_color(time_layer, GColorFromHEX(settings.timetextcolour));
  text_layer_set_text_color(date_layer, GColorFromHEX(settings.datetextcolour));
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  Tuple *tpstepgoal = dict_find(iter, MESSAGE_KEY_stepgoal);
  if(tpstepgoal) {
    settings.StepGoal = tpstepgoal->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }
  
  Tuple *buzzondisconnect = dict_find(iter, MESSAGE_KEY_buzzondisconnect);
  if(buzzondisconnect) {
    settings.buzzondisconnect = buzzondisconnect->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }
  
  Tuple *timecolour = dict_find(iter, MESSAGE_KEY_timecolour);
  if(timecolour) {
    settings.timecolour = timecolour->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }
  
  
  Tuple *datecolour = dict_find(iter, MESSAGE_KEY_datecolour);
  if(datecolour) {
    settings.datecolour = datecolour->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }
  
  Tuple *timetextcolour = dict_find(iter, MESSAGE_KEY_timetextcolour);
  if(timetextcolour) {
    settings.timetextcolour = timetextcolour->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }
  
  
  Tuple *datetextcolour = dict_find(iter, MESSAGE_KEY_datetextcolour);
  if(datetextcolour) {
    settings.datetextcolour = datetextcolour->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }

  Tuple *facecolour = dict_find(iter, MESSAGE_KEY_facecolour);
  if(facecolour) {
    settings.facecolour = facecolour->value->int32;

  // Save the new settings to persistent storage
  prv_save_settings();
  }

  
  // Battery meter

  persist_write_bool(AppKeyShowBattery, true);
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());
  // layer_set_hidden(bitmap_layer_get_layer(battery_icon_layer),false);
  layer_set_hidden(battery_layer,false);

}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, GColorBlack);
  // main_window = window;

  // Load custom fonts
    
  timeFont = PBL_IF_ROUND_ELSE(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_VISITOR_56)),fonts_load_custom_font(resource_get_handle(RESOURCE_ID_VISITOR_54)));
  
  dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DOTS_40));
  
  // dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DOTS_24));
    
  percentFont = PBL_IF_ROUND_ELSE(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DOTS_30)),fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DOTS_40)));

  // Create time TextLayer
  timecolour_layer = PBL_IF_ROUND_ELSE(text_layer_create(GRect(0, 50, 180, 43)), text_layer_create(GRect(0, 0, 0, 0)));
  layer_add_child(window_layer, text_layer_get_layer(timecolour_layer));
  
  time_layer = PBL_IF_ROUND_ELSE(text_layer_create(GRect(0, 30, 180, 70)), text_layer_create(GRect(1, 24, 146, 70)));
  text_layer_set_background_color(time_layer, GColorClear);
    
  text_layer_set_text_color(time_layer, GColorWhite);

  text_layer_set_font(time_layer, timeFont);
  PBL_IF_ROUND_ELSE(text_layer_set_text_alignment(time_layer, GTextAlignmentCenter),text_layer_set_text_alignment(time_layer, GTextAlignmentRight));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  // Create date TextLayer
  datecolour_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 94, 180, 53),GRect(0, 0, 0, 0)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(datecolour_layer));
  
  date_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 103, 180, 40),GRect(1, 66, 144, 50)));
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_font(date_layer, dateFont);
//  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  PBL_IF_ROUND_ELSE(text_layer_set_text_alignment(date_layer, GTextAlignmentCenter),text_layer_set_text_alignment(date_layer, GTextAlignmentRight));
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));

  // Create day TextLayer
  day_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 76, 180, 40),GRect(1, 96, 144, 40)));
  text_layer_set_background_color(day_layer, GColorClear);
  text_layer_set_text_color(day_layer, GColorWhite);
  text_layer_set_font(day_layer, dateFont);

  // text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);

  PBL_IF_ROUND_ELSE(text_layer_set_text_alignment(day_layer, GTextAlignmentCenter),text_layer_set_text_alignment(day_layer, GTextAlignmentRight));
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(day_layer));

  
  // Create step TextLayer
  stepcolour_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 23, 180, 26),GRect(0, 0, 0, 0)));
  text_layer_set_background_color(stepcolour_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(stepcolour_layer));
  
  step_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 6, 180, 42),GRect(1, 125, 144, 40)));
  text_layer_set_background_color(step_layer, GColorClear);
  text_layer_set_text_color(step_layer, GColorWhite);
  text_layer_set_font(step_layer, dateFont);
  // text_layer_set_text_alignment(step_layer, GTextAlignmentCenter);
  
  PBL_IF_ROUND_ELSE(text_layer_set_text_alignment(step_layer, GTextAlignmentCenter),text_layer_set_text_alignment(step_layer, GTextAlignmentRight));

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(step_layer));

  // Create xpercent TextLayer
  xpercent_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 148, 180, 23),GRect(0, 0, 0, 0)));
  text_layer_set_background_color(xpercent_layer, GColorClear);
  text_layer_set_text_color(xpercent_layer, GColorWhite);
  text_layer_set_font(xpercent_layer, percentFont);
  text_layer_set_text_alignment(xpercent_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(xpercent_layer));

// Create percent TextLayer
  percent_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 136, 180, 33),GRect(1, 2, 144, 40)));
  text_layer_set_background_color(percent_layer, GColorClear);
  text_layer_set_text_color(percent_layer, GColorWhite);
  text_layer_set_font(percent_layer, percentFont);
  // text_layer_set_text_alignment(percent_layer, GTextAlignmentCenter);

  PBL_IF_ROUND_ELSE(text_layer_set_text_alignment(percent_layer, GTextAlignmentCenter),text_layer_set_text_alignment(percent_layer, GTextAlignmentRight));

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(percent_layer));
 
  
  // Bluetooth icon
  //bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH);
  //bt_icon_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(GRect(73, 0, 30, 30),GRect(2, 2, 30, 30)));
  //bitmap_layer_set_compositing_mode(bt_icon_layer, GCompOpSet);
  //bitmap_layer_set_bitmap(bt_icon_layer, bt_icon_bitmap);
  //layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bt_icon_layer));

  // Create bt TextLayer
  bt_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, -8, 180, 30),GRect(1, 1, 80, 40)));
  text_layer_set_background_color(bt_layer, GColorClear);
  text_layer_set_text_color(bt_layer, GColorWhite);
  text_layer_set_font(bt_layer, percentFont);
  // text_layer_set_text_alignment(bt_layer, GTextAlignmentCenter);

  PBL_IF_ROUND_ELSE(text_layer_set_text_alignment(bt_layer, GTextAlignmentCenter),text_layer_set_text_alignment(bt_layer, GTextAlignmentLeft));
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(bt_layer));

  bluetooth_callback(connection_service_peek_pebble_app_connection());
  
  // Create xpercent TextLayer
  last_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(1, 172, 180, 10),GRect(0, 0, 0, 0)));
  text_layer_set_background_color(last_layer, GColorBlack);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(last_layer));
  
  // Battery icon
  // battery_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BATTERY);
  // charging_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHARGING);
  
  // battery_icon_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(GRect(60, 147, 24, 24),GRect(83, 15, 24, 24)));
  // bitmap_layer_set_compositing_mode(battery_icon_layer, GCompOpSet);
  // if(battery_plugged){
  //    bitmap_layer_set_bitmap(battery_icon_layer, charging_icon_bitmap);  
  // } else {
  //    bitmap_layer_set_bitmap(battery_icon_layer, battery_icon_bitmap);
  // }
  
  // layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_icon_layer));

  // Battery meter
  // battery_layer = layer_create(PBL_IF_ROUND_ELSE(GRect(63, 156, 16, 6),GRect(86, 24, 16, 6)));
  battery_layer = layer_create(PBL_IF_ROUND_ELSE(GRect(63, 156, 16, 6),GRect(0, 0, 0, 0)));

  // layer_set_update_proc(battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(window), battery_layer);

  //hidden for now, but still drawing.....
  
  // layer_set_hidden(bitmap_layer_get_layer(battery_icon_layer),true);
  layer_set_hidden(battery_layer,true);

  
  // subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
  
}

static void main_window_unload(Window *window) {
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(day_layer);
  text_layer_destroy(step_layer);
  text_layer_destroy(stepcolour_layer);
  text_layer_destroy(percent_layer);
  text_layer_destroy(xpercent_layer);
  text_layer_destroy(bt_layer);
  text_layer_destroy(last_layer);

  fonts_unload_custom_font(timeFont);
  fonts_unload_custom_font(dateFont);
  fonts_unload_custom_font(percentFont);
  
  // gbitmap_destroy(bg_bitmap);
  // gbitmap_destroy(bt_icon_bitmap);
  // bitmap_layer_destroy(bt_icon_layer);
  // gbitmap_destroy(battery_icon_bitmap);
  // bitmap_layer_destroy(battery_icon_layer);
  layer_destroy(battery_layer);
}


static void init() {
  main_window = window_create();

  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(main_window, true);
  
  prv_load_settings();
  
  app_message_register_inbox_received((AppMessageInboxReceived) inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });


  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());

  
}

static void deinit() {
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
