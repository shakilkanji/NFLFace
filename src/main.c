#include <pebble.h>
  
#define KEY_HOME_INFO 0
#define KEY_AWAY_INFO 1
#define KEY_NETWORK_INFO 2
#define KEY_DATE_INFO 3
#define KEY_DESIRED_TEAM 4
#define KEY_PERSIST_TEAM 5
#define KEY_TWENTY_FOUR_HOUR_FORMAT 6
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_home_layer;
static TextLayer *s_away_layer;
static TextLayer *s_network_layer;
static TextLayer *s_date_layer;

static GFont s_time_font;
static GFont s_scores_font;
static GFont s_info_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static bool twenty_four_hour_format = false;

static char home_layer_buffer[16];
static char away_layer_buffer[16];
static char network_layer_buffer[16];
static char date_layer_buffer[16];
static char desired_team_buffer[16];

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char s_time_buffer[16];

  if (twenty_four_hour_format) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
  } else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", tick_time);
  }
  
  if (!twenty_four_hour_format && s_time_buffer[0] == '0') {
    memmove(s_time_buffer, &s_time_buffer[1], sizeof(s_time_buffer) - 1);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_time_buffer);
}

static void set_time_format(bool box_checked) {
  twenty_four_hour_format = (box_checked) ? true : false;
}

static uint32_t choose_resource_id() {
  if (strcmp(desired_team_buffer, "ARI") == 0) {
    return RESOURCE_ID_ARI;
  }
  else if (strcmp(desired_team_buffer, "ATL") == 0) {
    return RESOURCE_ID_ATL;
  }
  else if (strcmp(desired_team_buffer, "BAL") == 0) {
    return RESOURCE_ID_BAL;
  }
  else if (strcmp(desired_team_buffer, "BUF") == 0) {
    return RESOURCE_ID_BUF;
  }
  else if (strcmp(desired_team_buffer, "CAR") == 0) {
    return RESOURCE_ID_CAR;
  }
  else if (strcmp(desired_team_buffer, "CHI") == 0) {
    return RESOURCE_ID_CHI;
  }
  else if (strcmp(desired_team_buffer, "CIN") == 0) {
    return RESOURCE_ID_CIN;
  }
  else if (strcmp(desired_team_buffer, "CLE") == 0) {
    return RESOURCE_ID_CLE2;
  }
  else if (strcmp(desired_team_buffer, "DAL") == 0) {
    return RESOURCE_ID_DAL;
  }
  else if (strcmp(desired_team_buffer, "DEN") == 0) {
    return RESOURCE_ID_DEN;
  }
  else if (strcmp(desired_team_buffer, "DET") == 0) {
    return RESOURCE_ID_DET;
  }
  else if (strcmp(desired_team_buffer, "GB") == 0) {
    return RESOURCE_ID_GB2;
  }
  else if (strcmp(desired_team_buffer, "HOU") == 0) {
    return RESOURCE_ID_HOU;
  }
  else if (strcmp(desired_team_buffer, "IND") == 0) {
    return RESOURCE_ID_IND2;
  }
  else if (strcmp(desired_team_buffer, "JAC") == 0) {
    return RESOURCE_ID_JAC;
  }
  else if (strcmp(desired_team_buffer, "KC") == 0) {
    return RESOURCE_ID_KC;
  }
  else if (strcmp(desired_team_buffer, "MIA") == 0) {
    return RESOURCE_ID_MIA;
  }
  else if (strcmp(desired_team_buffer, "MIN") == 0) {
    return RESOURCE_ID_MIN;
  }
  else if (strcmp(desired_team_buffer, "NE") == 0) {
    return RESOURCE_ID_NE;
  }
  else if (strcmp(desired_team_buffer, "NO") == 0) {
    return RESOURCE_ID_NO;
  }
  else if (strcmp(desired_team_buffer, "NYG") == 0) {
    return RESOURCE_ID_NYG;
  }
  else if (strcmp(desired_team_buffer, "NYJ") == 0) {
    return RESOURCE_ID_NYJ;
  }
  else if (strcmp(desired_team_buffer, "OAK") == 0) {
    return RESOURCE_ID_OAK;
  }
  else if (strcmp(desired_team_buffer, "PHI") == 0) {
    return RESOURCE_ID_PHI;
  }
  else if (strcmp(desired_team_buffer, "PIT") == 0) {
    return RESOURCE_ID_PIT;
  }
  else if (strcmp(desired_team_buffer, "STL") == 0) {
    return RESOURCE_ID_STL;
  }
  else if (strcmp(desired_team_buffer, "SD") == 0) {
    return RESOURCE_ID_SD;
  }
  else if (strcmp(desired_team_buffer, "SEA") == 0) {
    return RESOURCE_ID_SEA;
  }
  else if (strcmp(desired_team_buffer, "TB") == 0) {
    return RESOURCE_ID_TB3;
  }
  else if (strcmp(desired_team_buffer, "TEN") == 0) {
    return RESOURCE_ID_TEN;
  }
  else if (strcmp(desired_team_buffer, "WAS") == 0) {
    return RESOURCE_ID_WAS;
  }
  else {
    return RESOURCE_ID_SF;
  }
}

static void main_window_load(Window *window) {
  // Check to see if a desired team has already been selescted
  if (persist_exists(KEY_PERSIST_TEAM)) {
    persist_read_string(KEY_PERSIST_TEAM, desired_team_buffer, sizeof(desired_team_buffer));
  }
  
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(choose_resource_id());
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 0, 144, 46));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create GFont: credit www.dafont.com
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FRESHMAN_40));
  s_scores_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_20));
  s_info_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_14));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Create Home Layer
  s_home_layer = text_layer_create(GRect(0, 115, 72, 20));
  text_layer_set_background_color(s_home_layer, GColorClear);
  text_layer_set_text_color(s_home_layer, GColorWhite);
  text_layer_set_text_alignment(s_home_layer, GTextAlignmentCenter);
  
  // Create Away Layer
  s_away_layer = text_layer_create(GRect(72, 115, 72, 20));
  text_layer_set_background_color(s_away_layer, GColorClear);
  text_layer_set_text_color(s_away_layer, GColorWhite);
  text_layer_set_text_alignment(s_away_layer, GTextAlignmentCenter);

  // Create Network Layer
  s_network_layer = text_layer_create(GRect(0, 135, 144, 16));
  text_layer_set_background_color(s_network_layer, GColorClear);
  text_layer_set_text_color(s_network_layer, GColorWhite);
  text_layer_set_text_alignment(s_network_layer, GTextAlignmentCenter);

  // Create Date Layer
  s_date_layer = text_layer_create(GRect(0, 151, 144, 16));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  // Create second custom font, apply it and add to Window
  text_layer_set_font(s_home_layer, s_scores_font);
  text_layer_set_font(s_away_layer, s_scores_font);
  text_layer_set_font(s_network_layer, s_info_font);
  text_layer_set_font(s_date_layer, s_info_font);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_home_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_away_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_network_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Store data to memory
  persist_write_string(KEY_PERSIST_TEAM, desired_team_buffer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_scores_font);
  fonts_unload_custom_font(s_info_font);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy Time TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy NFL TextLayers
  text_layer_destroy(s_home_layer);
  text_layer_destroy(s_away_layer);
  text_layer_destroy(s_network_layer);
  text_layer_destroy(s_date_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get scores update every 10 minutes
  if(tick_time->tm_min % 10 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair, send persisting team to JS
    dict_write_cstring(iter, 4, desired_team_buffer);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_HOME_INFO:
      snprintf(home_layer_buffer, sizeof(home_layer_buffer), "%s", t->value->cstring);
      break;
    case KEY_AWAY_INFO:
      snprintf(away_layer_buffer, sizeof(away_layer_buffer), "%s", t->value->cstring);
      break;
    case KEY_NETWORK_INFO:
      snprintf(network_layer_buffer, sizeof(network_layer_buffer), "%s", t->value->cstring);
      break;
    case KEY_DATE_INFO:
      snprintf(date_layer_buffer, sizeof(date_layer_buffer), "%s", t->value->cstring);
      break;
    case KEY_DESIRED_TEAM:
      snprintf(desired_team_buffer, sizeof(desired_team_buffer), "%s", t->value->cstring);
      APP_LOG(APP_LOG_LEVEL_ERROR, "Desired team set as %s", desired_team_buffer);
      
      // Reload the window to refresh background image
      main_window_unload(s_main_window);
      main_window_load(s_main_window);
      break;
    case KEY_TWENTY_FOUR_HOUR_FORMAT:
      set_time_format(t->value->int8);
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Display full string
  text_layer_set_text(s_home_layer, home_layer_buffer);
  text_layer_set_text(s_away_layer, away_layer_buffer);
  text_layer_set_text(s_network_layer, network_layer_buffer);
  text_layer_set_text(s_date_layer, date_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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
